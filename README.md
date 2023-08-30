# About
NS is an object-oriented language with a minimal instruction set that simplifies programming and metaprogramming.

# Scopes
The base operation of NS is typical variable assignment like `x = 2` and callable assignment like `sqr(x) := {x*x}`. The last one defines predicates/methods, and we will talk about it later.

The main data structure is code blocks, enclosed in either parentheses or brackets. Code blocks can enclose multiple expressions separated by semicolons. The outcome of the last expression is its returned value, i.e., you can assign block outcomes to variables like this `z = {y=x+1; y*y}`. For example, for x = 1, we obtain z = 4. 

Whether to use brackets or parentheses depends on whether you want variables assignments inside the block to escape to the broader scope. F code safety, defer to brackets when creating intermediate variables, but otherwise prefer parentheses to avoid overheads.

# Objects and self-reference
NS has object-oriented features. new can be accessed from within code blocks to refer to their variable scope. Fun (object-oriented) things happen when outputting the new reference. For example, `point = {x=1; y=1; new}` creates an object with two fields x and y (we will tackle how to declare classes and constructors later).

Access fields of this object by calling `point.x`. Field access has lower priority than assignment. Therefore, if you want to assign this value to another variable, you need to write `temp = (point.x)`.
Notice that parentheses are a code block with just one statement inside and behave as we are used to. Without the parentheses, the above expression would read (temp = point).x. We go through this hassle to accommodate various powerful language features. One of these is that field access in  truth changes the scope in which its right-hand side is computed. This means that you can use it to perform complex operations within an object’s scope, as in `point.(x = x+1; z = 2)`
In simpler scenarios, the same convention will let you modify object contents without parentheses, as in `point.x = x+1`

# Callables
The final operation at the core of NS is the callable assignment, which defines any kind of predicate-based notation, including that of other programming languages. Basically, you need to write a textual expression to be matched, including spaces between predicates, and any arguments need to be enclosed parentheses. Arguments are added to the scope when running the methods/ For example, you can define a constructor for class objects like so: 

```
Point(x,y,z) := {norm squared:=x*x + y*y + z*z; new}
```

In this snippet, norm squared is a method name that includes spaces. For example write:

```
A = Point(0,0,1);
print(A.norm squared);
```

Method names need at least one space if they do not have any variables. This is needed to create a strict distinction between predicates and variables. The snippet also uses tuples of variables like (x,y,z), but keep in mind that NS considers the comma as a shorthand and converts the expression to (x) (y) (z) under the hood. This maintains the convention that the last value of code blocks is returned. Method definitions reside within their own scope.
Polymorphism is supported by declaring new methods with different arguments as previous ones, as in `Point(x,y) := Point(x,y,0);`. Use objects to pass keyword arguments or arguments that you don’t want to remain in the object’s scope.

