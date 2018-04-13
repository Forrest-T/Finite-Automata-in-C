#ifndef _DFA_PRINT_H_
#define _DFA_PRINT_H_

static const char preamble[] =
"\\documentclass{article}\n"
"\\usepackage[landscape,margin=0px,top=1cm]{geometry}"
"\\usepackage{tikz}\n"
"\\usepackage{environ}\n"
"\\usepackage{LinkedList}\n"
"\\usetikzlibrary{arrows}\n"
"\\usetikzlibrary{automata}\n"
"\\NewEnviron{automaton}{\n"
"    \\begin{tikzpicture}[->,>=stealth',shorten >=1pt,auto,node distance=2cm,thick,initial text={},double=]\n"
"    \\BODY\n"
"    \\end{tikzpicture}\n"
"}\n\n"
"\\begin{document}\n"
"\\begin{center}\n"
"    \\begin{automaton}\n";

static const char postamble[] =
"    \\end{automaton}\n"
"\\end{center}\n"
"\\end{document}\n";

static const char display[] =
"cd output"
"&& lualatex output.tex &>/dev/null"
"&& evince --fullscreen output.pdf &>/dev/null";

#endif
