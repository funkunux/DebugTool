LARGS := -lrt -DDEBUG
CFLAG := -std=c++11 -g


all:app.exe dbt elfdump 

app.exe:app.o msgQueue.hpp
	g++ $(CFLAG) -o $@ $< $(LARGS)

dbt:dbt.o msgQueue.hpp
	g++ $(CFLAG) -o $@ $< $(LARGS)

elfdump:elfdump.o
	g++ $(CFLAG) -o $@ $< $(LARGS)

%.o:%.cpp
	g++ $(CFLAG) -o $@ -c $< $(LARGS)

app.o:msgQueue.hpp
dbt.o:msgQueue.hpp
elfdump.o:ElfFile.h FileReader.h

clean:
	rm -f *.o app.exe dbt
