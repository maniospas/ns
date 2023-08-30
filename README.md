# About
NS is an object-oriented language with a minimal instruction set that simplifies programming and metaprogramming.

# Scopes
The base operation of NS is typical variable assignment like `x = 2` and callable assignment like `sqr(x) := {x*x}`. The last one defines callable formulas (e.g., methods), and we will talk about it later.

The main data structure is scope, enclosed in either parentheses or brackets. Scopes can enclose multiple expressions separated by semicolons. The outcome of the last expression is its returned value, i.e., you can assign block outcomes to variables like this `z = {y=x+1; y*y}`, where x = 1 yields z = 4. 

Whether to use brackets or parentheses depends on whether you want variables assignments inside the block to escape to the broader scope. F code safety, defer to brackets when creating intermediate variables, but otherwise prefer parentheses to avoid overheads.

# Objects and self-reference
NS has object-oriented features. `new` can be accessed from within scopes to refer to their variable scope. Fun (object-oriented) things happen when outputting the new reference. For example, `point = {x=1; y=1; new}` creates an object with fields x and y. We will tackle classes and constructors later. Get or set of this object on `point.x`. This actually enters the left-hand-side scope, which means that you can use it to edit objects like so:

```cpp
point.(x = x+1; z = 2);
```

You can also use bracket scopes to create objects within the scope of other objects 
(they can look at the latter's variables) like so:

```cpp
point = {x=1;y=2;z=3;new};
derived = point.{y=4;z=z;new}; // creates an object within the scope of point
point.(x=0;y=0;z=0); // edits the point
print(derived.x); // since x is not set in derived, it will be retrieved from its superscope and will be 0
print(derived.y); // this is directly set in the scope of derived and will be 4
print(derived.z); // this is also directly set for derived, and got the value point.z had at the time, that is, 3
```

# Formulas
The final operation at the core of NS is the assignment of callable formulas. These define any kind of predicate-based notation, including that of other programming languages. Basically, you need to write a textual expression to be matched, including spaces between predicates, and any arguments need to be enclosed parentheses. Arguments are added to the scope of running formulas. For example, you can define a constructor for class objects like so: 

```cpp
Point(x,y,z) := {
    norm squared:=x*x + y*y + z*z; 
    new
}
```

In this snippet, norm squared is a method name that includes spaces. For example write:

```cpp
A = Point(0,0,1);
print(A.norm squared);
```

Method names need at least one space if they do not have any variables. This is needed to create a strict distinction between predicates and variables. The snippet also uses tuples of variables like (x,y,z), but keep in mind that NS considers the comma as a shorthand and converts the expression to (x) (y) (z) under the hood. This maintains the convention that the last value of scopes is returned. Method definitions reside within their own scope.
Polymorphism is supported by declaring new methods with different arguments as previous ones, like so: 

```cpp
Point(x,y) := Point(x,y,0);
```

Use objects to pass keyword arguments or arguments that you don’t want to remain in the object’s scope.

