# PiLang AST nodes
This is a documentation introducing PiLang AST node kinds.

## PiLang node kinds

### `program` node _shell_
__Production__: `program: functions` <br/>

### `functions` node _list_
__Production__: `functions function | <empty>` <br/>
__ANK__: `ANK_LIST` <br/>
__ANS__: `ANS_FUNCTIONS` <br/>
__Children__: `function`s <br/>

### `function` node _entity_
__Production__: 
`TK_FUNCTION TK_ID TK_TAKES id_list TK_RETURNS id_list function_body`
<br/>
__ANK__: `ANK_TRIPLE_CHILD_WDATA` <br/>
__ANS__: `ANS_FUNCTION` <br/>
__Data__: `TK_ID` identifier name <br/>
__Child0__: params `id_list` <br/>
__Child1__: return values `id_list` <br/>
__Child2__: `function_body` <br/>

### `function_body` node _wrapper_
__Production__: `TK_BEGIN statements TK_END` <br/>
__ANK__: `ANK_SINGLE_CHILD` <br/>
__ANS__: `ANS_FUNCTION_BODY` <br/>
__Child0__: `statements` <br/>

### `statements` node _list_
__Production__: `statements statement | <empty>` <br/>
__ANK__: `ANK_LIST` <br/>
__ANS__: `ANS_STATEMENTS` <br/>
__Children__: `statement`s <br/>

### `statement` node _shell_
__Production__: 
`expr_statement | if_statement | while_statement | for_statement | `
`empty_statement` <br/>

### `expr_statement` node _shell_
__Production__: `expr TK_SYM_SEMI` <br/>

### `expr` node _shell_
__Production__: `assign_expr`

### `assign_expr` node _entity_
