grammar rectangle;

document    // Def | Identifier
    : componentDefination EOF   // Def
    | componentInstance EOF     // Identifier
    ;

componentDefination     // Def
    : Def Identifier LBrace memberItemList RBrace
    ;

memberItemList  // Int | Void | Float | String | List | Enum | Identifier
    : memberItem*    // Int | Void | Float | String | List | Enum | Identifier
    ;

memberItem  // Int | Void | Float | String | List | Enum | Identifier
    : propertyDefination    // Int | Float | String | List
    | functionDefination    // Int | Void | Float | String | List | Identifier
    | enumDefination        // Enum
    ;

propertyDefination  // Int | Float | String | List
    : propertyType Identifier Colon initializer  // Int | Float | String | List
    ;

propertyType    // Int | Float | String | List
    : Int
    | Float
    | String
    | listType  // List
    ;

type    // Int | Void | Float | String | List | Identifier
    : Int
    | Void
    | Float
    | String
    | listType  // List
    | Identifier
    ;

listType    // List
    : List Lt propertyType Gt
    ;

literal // StringLiteral | NumberLiteral
    : StringLiteral
    | NumberLiteral
    ;

functionDefination  // Int | Void | Float | String | List | Identifier
    : type Identifier LParen paramList? RParen compoundStatement    // Int | Void | Float | String | List | Identifier
    ;

paramList   // Int | Void | Float | String | List | Identifier
    : paramItem (Comma paramItem)*  // Int | Void | Float | String | List | Identifier
    ;

paramItem   // Int | Void | Float | String | List | Identifier
    : type Identifier   // Int | Void | Float | String | List | Identifier
    ;

compoundStatement   // LBrace
    : LBrace blockItemList RBrace
    ;

blockItemList   // Int | Void | Float | String | List | Identifier | LBrace | If | While | Continue | Break | Return | StringLiteral | NumberLiteral | LParen
    : blockItem*    // Int | Void | Float | String | List | Identifier | LBrace | If | While | Continue | Break | Return | StringLiteral | NumberLiteral | LParen
    ;

blockItem   // Int | Void | Float | String | List | Identifier | LBrace | If | While | Continue | Break | Return | StringLiteral | NumberLiteral | LParen
    : declaration   // Int | Void | Float | String | List | Identifier
    | statement     // LBrace | If | While | Continue | Break | Return | Identifier | StringLiteral | NumberLiteral | LParen
    ;

declaration // Int | Void | Float | String | List | Identifier
    : type Identifier (Assign initializer)? Semicolon   // Int | Void | Float | String | List | Identifier
    ;

initializer // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not | LBrace
    : expression                        // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
    | LBrace initializerList? RBrace    // LBrace
    ;

initializerList // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not | LBrace
    : initializer (Comma initializer)*  // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not | LBrace
    ;

expression  // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
    : logicalOrExpression                   // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
    ;

logicalOrExpression // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
    : logicalAndExpression (OrOr logicalAndExpression)* // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
    ;

logicalAndExpression    // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
    : equalityExpression (AndAnd equalityExpression)*   // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
    ;

equalityExpression  // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
    : relationalExpression  // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
        (
            ( Equal
            | NotEqual
            ) relationalExpression
        )*
    ;

relationalExpression    // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
    : additiveExpression    // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
        (
            ( Lt
            | Gt
            | Le
            | Ge
            ) additiveExpression
        )*
    ;

additiveExpression  // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
    : multiplicativeExpression  // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
        (
            ( Plus
            | Minus
            ) multiplicativeExpression
        )*
    ;

multiplicativeExpression    // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
    : unaryExpression   // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
        (
            ( Star
            | Slash
            | Remainder
            ) unaryExpression
        )*
    ;

unaryExpression // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
    : postfixExpression             // Identifier | StringLiteral | NumberLiteral | LParen
    | unaryOperator unaryExpression // Plus | Minus | Not
    ;

unaryOperator   // Plus | Minus | Not
    : Plus 
    | Minus
    | Not
    ;

postfixExpression   // Identifier | StringLiteral | NumberLiteral | LParen
    : primaryExpression // Identifier | StringLiteral | NumberLiteral | LParen
        ( LBracket expression RBracket
        | LParen argumentExpressionList? RParen
        | Dot Identifier
        )*
    ;

primaryExpression   // Identifier | StringLiteral | NumberLiteral | LParen
    : Identifier
    | literal                     // StringLiteral | NumberLiteral
    | LParen expression RParen
    ;

argumentExpressionList  // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
    : expression (Comma expression)*    // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
    ;

statement   // LBrace | If | While | Continue | Break | Return | Identifier | StringLiteral | NumberLiteral | LParen
    : compoundStatement                                 // LBrace
    | selectionStatement                                // If
    | iterationStatement                                // While
    | jumpStatement                                     // Continue | Break | Return
    | assignmentStatement                               // Identifier | StringLiteral | NumberLiteral | LParen
    ;

selectionStatement  // If
    : If LParen expression RParen compoundStatement
        ( Else
            ( compoundStatement     // LBrace
            | selectionStatement    // If   
            )
        )?
    ;

iterationStatement  // While
    : While LParen expression RParen compoundStatement  // While
    ;

jumpStatement   // Continue | Break | Return
    : Continue Semicolon
    | Break Semicolon
    | Return expression? Semicolon
    ;

assignmentStatement // Identifier | StringLiteral | NumberLiteral | LParen
    : postfixExpression (Assign expression)? Semicolon  // Identifier | StringLiteral | NumberLiteral | LParen
    ;

enumDefination  // Enum
    : Enum Identifier LBrace enumConstantList RBrace
    ;

enumConstantList    // Identifier
    : enumConstant (Comma enumConstant)*    // Identifier
    ;

enumConstant    // Identifier
    : Identifier
    ;

componentInstance   // Identifier
    : Identifier LBrace bindingItemList RBrace
    ;

bindingItemList // Identifier ( Colon | Dot | LBrace)
    : bindingItem+  // Identifier ( Colon | Dot | LBrace)
    ;

bindingItem // Identifier ( Colon | Dot | LBrace)
    : Identifier Colon initializer                  // Identifier Colon
    | componentInstance                                 // Identifier LBrace
    ;

If : 'if';
Def : 'def';
Int : 'int';
Else : 'else';
Enum : 'enum';
List : 'list';
Void : 'void';
Break : 'break';
Float : 'float';
While : 'while';
Return : 'return';
String : 'string';
Continue : 'continue';
LBrace : '{';
RBrace : '}';
LBracket : '[';
RBracket : ']';
LParen : '(';
RParen : ')';
Comma : ',';
Colon : ':';
Quote : '"';
SingleQuote : '\'';
AndAnd : '&&';
OrOr : '||';
Not : '!';
Plus : '+';
Minus : '-';
Star : '*';
Slash : '/';
Remainder : '%';
Dot : '.';
Ge : '>=';
Le : '<=';
Gt : '>';
Lt : '<';
Equal : '==';
NotEqual : '!=';
Assign : '=';
Semicolon : ';';
StringLiteral
    : '\'' SChar+ '\''
    | '"' SChar+ '"'
    ;
NumberLiteral
    : DigitSequence
    | DigitSequence '.' DigitSequence
    ;
Identifier
    : IdentifierNondigit
      ( IdentifierNondigit
      | Digit
      )*
    ;

fragment
SChar
    : ~["\\\r\n]
    | EscapeSequence
    ;

fragment
EscapeSequence
    : '\\' ['"?abfnrtv\\]
    ;

fragment
DigitSequence
    : Digit+
    ;

fragment
Digit
    : [0-9]
    ;

fragment
IdentifierNondigit
    : [a-zA-Z_]
    ;