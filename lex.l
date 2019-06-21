%{
#include <string>
#include <map>
#include <iostream>
int line = 0;
int numCount = 0;
%}

digit	[0~9];
letter	[A~Za~z];
identifier	{letter}({letter}|{digit})*;
number	[-+]?{digit}*.?{digit}+;
%%
auto	{return "AUTO";}
break	{return "BREAK";}
case	{return "CASE";}
char	{return "CHAR";}
const	{return "CONST";}
continue	{return "CONTINUE";}
default	{return "DEFAULT";}
do	{return "DO";}
double	{return "DOUBLE";}
else	{return "ELSE";}
enum	{return "ENUM";}
extern	{return "EXTERN";}
float	{return "FLOAT";}
for	{return "FOR";}
goto	{return "GOTO";}
if	{return "IF";}
int	{return "INT";}
long	{return "LONG";}
register	{return "";}
return	{return "RETURN";}
short	{return "SHORT";}
signed	{return "SIGNED";}
sizeof	{return "SIZEOF";}
static	{return "STATIC";}
struct	{return "STRUCT";}
switch	{return "SWITCH";}
typedef	{return "TYPEDEF";}
union	{return "UNION";}
unsigned	{return "UNSIGNED";}
void	{return "VOID";}
volatile	{return "VOLATILE";}
while	{return "WHILE";}
{identifier}	{return "IDENTIFIER";}
{digit}+	{return "CONSTANT";}
{number}	{return "CONSTANT";}
"[^"]*"	{return "STRING_LITERAL";}
...	{return "ELLIPSIS";}
>>=	{return "RIGHT_ASSIGN";}
<<=	{return "LEFT_ASSIGN";}
+=	{return "ADD_ASSIGN";}
-=	{return "SUB_ASSIGN";}
*=	{return "MUL_ASSIGN";}
/=	{return "DIV_ASSIGN";}
%=	{return "MOD_ASSIGN";}
&=	{return "AND_ASSIGN";}
^=	{return "XOR_ASSIGN";}
|=	{return "OR_ASSIGN";}
>>	{return "RIGHT_OP";}
<<	{return "LEFT_OP";}
++	{return "INC_OP";}
--	{return "DEC_OP";}
->	{return "PTR_OP";}
&&	{return "AND_OP";}
||	{return "OR_OP";}
<=	{return "LE_OP";}
>=	{return "GE_OP";}
==	{return "EQ_OP";}
!=	{return "NE_OP";}
;	{return ";";}
{	{return "{";}
}	{return "}";}
,	{return ",";}
:	{return ":";}
=	{return "=";}
(	{return "(";}
)	{return ")";}
[	{return "[";}
]	{return "]";}
.	{return ".";}
&	{return "&";}
!	{return "!";}
~	{return "~";}
-	{return "-";}
+	{return "+";}
*	{return "*";}
/	{return "/";}
%	{return "%";}
<	{return "<";}
>	{return ">";}
^	{return "^";}
|	{return "|";}
?	{return "?";}
//[^\n]*	{return "SINGLECOMMENT";}
/\*([^*]|\n|(\*+([^*/]|\n)))*\*+/	{return "BLOCKECOMMENT";}
%%

int
yywrap()
{
	return 1; /* tell scanner no further files need to be processed */
}
