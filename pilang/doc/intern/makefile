all: gast.html pilang-ast.html memmodel.html

gast.html: gast.md
	markdown gast.md > gast.html
	
pilang-ast.html: pilang-ast.md
	markdown pilang-ast.md > pilang-ast.html

memmodel.html: memmodel.md
	markdown memmodel.md > memmodel.html

clean:
	rm -rf *.html
