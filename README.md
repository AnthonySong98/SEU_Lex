**SEU_Lex**的执行步骤如下：

1. 打开终端，切换当前工作目录。

2. 输入`cmake .` 然后再输入`make all`，编译项目。

3. 输入`./Seu_Lex lex.l`，根据 `lex.l`文件，产生词法分析器**SEU_Lex**的C++代码文件 `Lex.yy.cpp`。

4. 输入 `g++ -std=c++11 Lex.yy.cpp -o Lexer`，编译生成词法分析器**SEU_Lex**的可执行文件。

5. 输入 `./Lexer testcase.cpp `，对测试代码文件进行词法分析，测试**SEU_Lex**的表现。

