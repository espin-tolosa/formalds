from typing import List

#clang -fno-color-diagnostics -Xclang -ast-dump -fsyntax-only example1.c

class Loc:
    def __init__( self, file: str, lc0: int, lc1: int ):
        self.file: str = file
        self.lc0: int  = lc0
        self.lc1: int  = lc1

class ParamVarDecl:
    def __init__( self, loc: Loc, label: str, ctype: str ):
        self.loc: Loc     = loc
        self.label: str   = label
        self.ctype: str   = ctype

class Stmt:
    def __init__( self, loc: Loc ):
        self.loc: Loc = loc

class DeclStmt( Stmt ):
    def __init__( self, loc: Loc, label: str, ctype: str ):
        super().__init__( loc )
        self.label: str   = label
        self.ctype: str   = ctype

class IfStmt( Stmt ):
    def __init__( self, loc: Loc, condition: str, then_stmt: Stmt, else_stmt: Stmt | None ):
        super().__init__( loc )
        self.condition: str        = condition
        self.then_stmt: Stmt       = then_stmt
        self.else_stmt: Stmt | None = else_stmt

class WhileStmt( Stmt ):
    def __init__( self, loc: Loc, condition: str, body: Stmt ):
        super().__init__( loc )
        self.condition: str = condition
        self.body: Stmt     = body

class ForStmt( Stmt ):
    def __init__( self, loc: Loc, init: Stmt | None, cond: str | None, inc: Stmt | None, body: Stmt ):
        super().__init__( loc )
        self.init: Stmt | None = init
        self.cond: str | None  = cond
        self.inc: Stmt | None  = inc
        self.body: Stmt        = body

class CompoundStmt( Stmt ):
    def __init__( self, loc: Loc, stmts: List[Stmt] ):
        super().__init__( loc )
        self.stmts: List[Stmt] = stmts

class FunctionDecl:
    def __init__( self, loc: Loc, params: List[ParamVarDecl], compound_stmt: CompoundStmt ):
        self.loc: Loc                        = loc
        self.params: List[ParamVarDecl]      = params
        self.compound_stmt: CompoundStmt     = compound_stmt

class ASTVisitor:
    def visit( self, node ):
        method_name = "visit_" + node.__class__.__name__
        visitor = getattr( self, method_name, self.generic_visit )
        return visitor( node )

    def generic_visit( self, node ):
        raise NotImplementedError(
            f"No visit_{node.__class__.__name__} method"
        )

class ASTPrinter( ASTVisitor ):
    def __init__( self ):
        self.indent: int = 0

    def _print( self, text: str ):
        print( "  " * self.indent + text )

    def visit_FunctionDecl( self, node: FunctionDecl ):
        self._print( "FunctionDecl" )
        self.indent += 1

        self._print( "Params:" )
        self.indent += 1
        for param in node.params:
            self.visit( param )
        self.indent -= 1

        self._print( "Body:" )
        self.indent += 1
        self.visit( node.compound_stmt )
        self.indent -= 1

        self.indent -= 1

    def visit_ParamVarDecl( self, node: ParamVarDecl ):
        self._print(
            f"ParamVarDecl {node.label} : {node.ctype}"
        )

    def visit_CompoundStmt( self, node: CompoundStmt ):
        self._print( "CompoundStmt" )
        self.indent += 1
        for stmt in node.stmts:
            self.visit( stmt )
        self.indent -= 1

    def visit_DeclStmt( self, node: DeclStmt ):
        self._print(
            f"DeclStmt {node.label} : {node.ctype}"
        )

    def visit_IfStmt( self, node: IfStmt ):
        self._print( f"IfStmt (cond = {node.condition})" )
        self.indent += 1

        self._print( "Then:" )
        self.indent += 1
        self.visit( node.then_stmt )
        self.indent -= 1

        if node.else_stmt is not None:
            self._print( "Else:" )
            self.indent += 1
            self.visit( node.else_stmt )
            self.indent -= 1

        self.indent -= 1

    def visit_WhileStmt( self, node: WhileStmt ):
        self._print( f"WhileStmt (cond = {node.condition})" )
        self.indent += 1
        self.visit( node.body )
        self.indent -= 1

    def visit_ForStmt( self, node: ForStmt ):
        self._print( "ForStmt" )
        self.indent += 1

        if node.init:
            self._print( "Init:" )
            self.indent += 1
            self.visit( node.init )
            self.indent -= 1

        if node.cond:
            self._print( f"Cond: {node.cond}" )

        if node.inc:
            self._print( "Inc:" )
            self.indent += 1
            self.visit( node.inc )
            self.indent -= 1

        self._print( "Body:" )
        self.indent += 1
        self.visit( node.body )
        self.indent -= 1

        self.indent -= 1

class LLRVisitor:
    def __init__( self, func_name: str, return_type: str ):
        self.func_name: str = func_name
        self.return_type: str = return_type
        self.lines: list[str] = []

    def visit( self, node ):
        method_name = "visit_" + node.__class__.__name__
        visitor = getattr( self, method_name, self.generic_visit )
        visitor( node )

    def generic_visit( self, node ):
        raise NotImplementedError(
            f"No LLR rule for {node.__class__.__name__}"
        )

    def visit_FunctionDecl( self, node: FunctionDecl ):
        # Interface requirement
        params = ", ".join(
            f"{p.ctype} {p.label}" for p in node.params
        )

        self.lines.append(
            f"The function {self.func_name} shall return {self.return_type} "
            f"and receive the arguments: {params}."
        )

        # Behavioral requirements header
        self.lines.append(
            f"The function {self.func_name} shall:"
        )

        # Visit function body
        self.visit( node.compound_stmt )

    def visit_CompoundStmt( self, node: CompoundStmt ):
        for stmt in node.stmts:
            self.visit( stmt )

    def visit_DeclStmt( self, node: DeclStmt ):
        self.lines.append(
            f"- store the value 0 into variable {node.label}"
        )

    def visit_CallInitDeclStmt( self, node ):
        args = ", ".join( str(a) for a in node.args )
        self.lines.append(
            f"- store the result of calling {node.callee}({args}) "
            f"into variable {node.label}"
        )

# Usage

# source location helpers
loc_func   = Loc( "example.c", 1, 10 )
loc_param1 = Loc( "example.c", 1, 15 )
loc_param2 = Loc( "example.c", 1, 22 )
loc_decl1  = Loc( "example.c", 3, 5 )
loc_decl2  = Loc( "example.c", 4, 5 )
loc_block  = Loc( "example.c", 2, 9 )

# function parameters
param_x = ParamVarDecl(
    loc   = loc_param1,
    label = "x",
    ctype = "int"
)

param_y = ParamVarDecl(
    loc   = loc_param2,
    label = "y",
    ctype = "int"
)

# statements inside function body
decl_a = DeclStmt(
    loc   = loc_decl1,
    label = "a",
    ctype = "int"
)

decl_b = DeclStmt(
    loc   = loc_decl2,
    label = "b",
    ctype = "float"
)

# compound statement (function body)
body = CompoundStmt(
    loc   = loc_block,
    stmts = [
        decl_a,
        decl_b,
    ]
)

# function declaration
func = FunctionDecl(
    loc            = loc_func,
    params         = [ param_x, param_y ],
    compound_stmt  = body
)

llr = LLRVisitor(
    func_name   = "example",
    return_type = "int"
)

llr.visit( func )

for line in llr.lines:
    print( line )
