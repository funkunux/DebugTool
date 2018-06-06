LARGS := -lrt -DDEBUG


all:app.exe dbt

app.exe:app.o msgQueue.hpp
	g++ -o $@ $< $(LARGS)

dbt:dbt.o msgQueue.hpp
	g++ -o $@ $< $(LARGS)

%.o:%.cpp
	g++ -o $@ -c $< $(LARGS)

app.o:msgQueue.hpp
dbt.o:msgQueue.hpp

clean:
	rm -f *.o app.exe dbt
