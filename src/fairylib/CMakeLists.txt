# add the executable
add_library(fairylib fairytale.cpp fairytale.h
					 fairytale_core.cpp fairytale_core.h
					 parsing.cpp parsing.h
					 standard_library.cpp standard_library.h
					 string_table.cpp string_table.h
					 fairy_object.cpp fairy_object.h
					 auto_ref.cpp auto_ref.h
					 rules.cpp rules.h
					 runtime.cpp runtime.h
					 lexer.cpp lexer.h
					 abstract_syntax_tree.cpp abstract_syntax_tree.h
					 config.cpp config.h
					 )