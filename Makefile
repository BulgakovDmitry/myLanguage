.SILENT:

#--------------------------------------------------------------------------------------------------
COMPILER = g++
#--------------------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------------------
FRON = frontend/
MIDD = middleend/
BACK = backend/
COMM = common/
SRC  = src/
HPP  = headers/
LIB  = myLib/
VEC  = vector/
SPU  = spu/
PROC = proc/
ASM  = asm/
STK  = stack/
OBJ  = obj/
#--------------------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------------------
INCLUDE_FLAGS = -I$(LIB) -I$(VEC) -I$(STK) -I$(HPP) 
SANITAZER     = -fsanitize=address
SFML_FLAGS    = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio 
GDB           = -g
OPTIMIZ_OF    = -O0

FLAGS		  = -D _DEBUG -ggdb3 -std=c++17 $(OPTIMIZ_OF) -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations \
			    -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wctor-dtor-privacy -Wempty-body -Wfloat-equal          \
			    -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd  \
				-Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel     \
				-Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override            \
				-Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros              \
				-Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector        \
				$(INCLUDE_FLAGS)                                                                                                             \
				$(SANITAZER)
#--------------------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------------------
MYLIB_OBJ     = $(OBJ)myLib.o
VECTOR_OBJ    = $(OBJ)vector.o
STACK_OBJ     = $(OBJ)stack.o $(OBJ)myLib.o
COMMON_OBJ    = $(OBJ)tree.o
FRONTEND_OBJ  = $(OBJ)lexicalAnalysis.o $(OBJ)syntaxAnalysis.o
MIDDLEEND_OBJ = $(OBJ)optimizations.o
BACKEND_OBJ   = $(OBJ)codeGenerator.o
PROC_OBJ      = $(OBJ)proc.o $(OBJ)mainProc.o  $(OBJ)myLib.o
ASM_OBJ       = $(OBJ)asm.o $(OBJ)mainAsm.o $(OBJ)myLib.o
#--------------------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------------------
spu: asm proc

spu_: asm_ proc_

asm_: asm
	./asm.out

proc_: proc
	./proc.out

language_: language
	./language.out

run: language spu

run_: language_ spu_
#--------------------------------------------------------------------------------------------------
     

#--------------------------------------------------------------------------------------------------
clean: 
	rm -f $(OBJ)*
#--------------------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------------------
language: $(MYLIB_OBJ) $(VECTOR_OBJ) $(COMMON_OBJ) $(FRONTEND_OBJ) $(MIDDLEEND_OBJ) $(BACKEND_OBJ) $(OBJ)myLanguageMain.o 
	$(COMPILER) $^ -o language.out $(FLAGS)

asm: $(ASM_OBJ)
	$(COMPILER) $^ -o asm.out $(FLAGS)

proc: $(STACK_OBJ) $(PROC_OBJ) 
	$(COMPILER) $^ -o proc.out $(FLAGS)
#--------------------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------------------
$(OBJ)%.o : $(LIB)%.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@

$(OBJ)%.o : $(VEC)%.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@	

$(OBJ)%.o : $(STK)%.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@

$(OBJ)%.o : $(COMM)$(SRC)%.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@

$(OBJ)%.o : $(FRON)$(SRC)%.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@

$(OBJ)%.o : $(MIDD)$(SRC)%.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@

$(OBJ)%.o : $(BACK)$(SRC)%.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@

$(OBJ)%.o : $(SPU)$(ASM)$(SRC)%.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@

$(OBJ)%.o : $(SPU)$(PROC)$(SRC)%.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@

$(OBJ)%.o : %.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@
#--------------------------------------------------------------------------------------------------