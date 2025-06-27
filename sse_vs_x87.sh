#!/bin/bash

# Step 1: Write the code to a file
cat > test_sse_vs_x87.c << 'EOF'
#include <stdio.h>
#include <math.h>

double compute(double a, double b) {
    return (a + b) * (a - b);
}

#include <stdio.h>

__attribute__((noipa)) static int opaque(int i) { return i; }

int x87_instability()
{
    int z = opaque(1) + 0x1p-60 == 1;

    printf("z = %d\n", z);
    if (z)
        puts("z is one");
}

void x87_precision_issue() {
    double a = 1.0;
    double b = 1e-16;
    double c = (a + b) - a;
    printf("Expected: 1e-16, Actual: %.17g\n", c);
}

int main() {
    double x = 1.0000001;
    double y = 1.0000000;
    double result = compute(x, y);
    printf("Result of (x + y) * (x - y) = %.17g\n", result);

    x87_precision_issue();

    x87_instability();

    return 0;
}
EOF

# Step 2: Compile with x87 (default for -m32)
gcc -m64 -mfpmath=387 -O3 test_sse_vs_x87.c -o test_x87_64

# Step 3: Compile with SSE
gcc -m64 -O0 -mfpmath=sse -msse2 -mno-80387 test_sse_vs_x87.c -o test_sse

# Step 4: Run both versions
echo "=== Running with x87 (default -m32) ==="
./test_x87_64

echo ""
echo "=== Running with SSE (-mfpmath=sse) ==="
./test_sse
