# About
NS is an interpreted language that treats variable scopes as objects and vice versa.

**Author:** Emmanouil (Manios) Krasanakis<br>
**License:** Apache 2<br>
**Status:** The core language interpreter is still being developed


1. [Setup](#setup-windows)
2. [Scopes, self-reference, objects](#scopes-self-reference-objects)
3. [Formulas, methods, constructors](#formulas-methods-constructors)
4. [Fallfronts and fallbacks (resolve variable value conflict)](#fallfronts-and-fallbacks-resolve-variable-value-conflict)
5. [Examples](#examples)


# Setup (windows)
*The language is still under development, and only a windows distribution is available.*

Download the interpreter executable `ns.exe` and add it to your path. 
Run a source code file `file.ns` with the following command line instruction.


```
ns file.ns
```

If you do not provide a file or run the interpreter by double-clicking it, 
`cli.ns` will be interpreted (download it alongside the executable). 
This is a command line interface for NS.


# Scopes, self-reference, objects
The base operation of NS is typical variable assignment like `x = 2` and callable assignment like `sqr(x) := {x*x}`. The last one defines callable formulas (e.g., methods), and we will talk about it later.

The main data structure is a variable scope, which is enclosed in either parentheses or brackets. A scope can enclose multiple expressions separated by semicolons. The outcome of the last expression is the scope's returned value, i.e., you can assign block outcomes to variables like this `z = {y=x+1; y*y}`, where x = 1 yields z = 4.

Whether to use brackets or parentheses depends on whether you want any variable assignments inside the scope to also be assigned to the superscope in which you are genering. For code safety, defer to brackets when creating intermediate variables, but otherwise prefer parentheses to avoid overheads - everything is written once and the language implements scope inheritance internally.

NS has object-oriented features. `new` refers to the current scope itself and `super` to the parent scope generating it. Fun (object-oriented) things happen when outputting the new reference. For example, `point = {x=1; y=1; new}` creates an object with fields x and y. We will tackle classes and constructors later. Get or set of this object can be performed on `point.x`. This actually enters the left-hand-side scope, which means that you can use it to edit objects like so:

```cpp
point.(x = x+1; z = 2);
```

You can also use bracket scopes to create objects within the scope of other objects 
(they can look at the latter's variables) like so:

```cpp
point = {x=1;y=2;z=3;new};
derived = point.{y=4;z=z;new}; // creates an object within the scope of A
point.(x=0;y=0;z=0); // edits the point (needs parenthesis for edits to be kept)
print(derived.x); // since x is not set in derived, it will be retrieved from its superscope and will be 0
print(derived.y); // this is directly set in the scope of derived and will be 4
print(derived.z); // this is also directly set for derived, and got the value point.z had at the time, that is, 3
print(derived.super); // this is the superscope of derived, which is the object point
```

# Formulas, methods, constructors
The final NS core operation is assignment of callable formulas. These define any kind of predicate-based notation.
Basically, you need to write a textual expression to be matched *including expressions with multiple predicates seperated by spaces*, and any arguments need to be enclosed parentheses. When formulas run, they do so under a scope of their argument
values. For example, you can define a constructor for class objects like so: 

```cpp
Point(x,y,z) := { // x, y, z are automatically accessed
    new
}
```

If formula implementations are not scoped into brackets, internal assignments are transferred 
to their superscope too per normal (the arguments themselves are never assigned, so they will
not leak). For example `set f(value):=(f=value)` is equivalent to `set f(value):={super.super.f=value}`,
where in the last expression the first superscope are the arguments and only this one's supercope
is the object.


```cpp
Point(x,y,z) := { // x, y, z are automatically accessed
    norm squared:=x*x + y*y + z*z; 
    shrink(a) := (x=x*a;y=y*a;z=z*a;); // if you want to keep track of the last argument value within Point set a=a
    new
}
```

In this snippet, norm squared is a method name that includes spaces. For example write:

```cpp
A = Point(0,0,1);
print(A.norm squared);
```

The snippet uses tuples of variables like (x,y,z), but keep in mind that NS considers the comma as a shorthand and converts the expression to (x) (y) (z) under the hood. This maintains the convention that the last value of scopes is returned. Method definitions reside within their own scope.
Polymorphism is supported by declaring new methods with different arguments as previous ones, like so: 

```cpp
Point(x,y) := Point(x,y,0);
```

Use objects to pass keyword arguments or arguments that you don’t want to remain in the object’s scope.


# Fallfronts and fallbacks (resolve variable value conflict)

We previously used `.` to enter a scope and both edit its contents and retrieve fields.
This is referred to as scope *access*. But, when a variable or formula is found
in both the entered and entrant scopes (e.g., because the entrant is bracketed and the 
assigned variable value does not trickle down to a common ancestor),
NS creates an exception due to ambiguity.
Resolve this by replacing the access with either a *fallfront* (#) or *fallback* (:) operator.

Of the two, *fallfront* means
that any variable values will first be looked at the scope being entered from. For example, the 
snippet `a:={x=0;new};{y=5;a.x=y};print(a.x)` will correctly print 5. But this makes it 
tricky reading fields or fomulas already defined in the accesor scope.
For example, the snippet
`a={x=0;new};x=5;print(a.x)` will print 5, even if internally the value x of
a still holds value 0, because the fallfront operator overrides names.


To prioritize internal variable values or formula implementations 
for accessed scopes, use the *fallback* operator `:`.
This has the inverse risk of not setting scope members if they also exist on the scope
being entered from.


:bulb: To avoid unexpected behavior, use fallfronts
to safe write to scopes, and fallbacks to safe read from scopes.
Best practices are enforced by usign simple access, but sometimes
the other two options are needed to perform complex stuff.


# Examples


The following example shows how to override a formula using the fallback operation.
Basically, we store a `base` scope that will help us define a new formula vermula
in the new one `stricter` by just assigning to the formula in the latter and fallbacking
on the base to implement it. We can then fallback to `stricter` to run the formula
there.


```cpp
stricter = {
    base=super; // alternative have base=new; on the top level and pop(super) here to minimize the logic
    (x)<(y) := {
        base: ((x)<((y)-(1))) // computation under the system scope
    };
    new
};
comp = stricter:(1)<(2); // test a comparison under the new scope
print(comp)
```

:bulb: In both cases, using a fallfront (.) instead of a fallback (:) would fail; 
it would respectively create an infinite recursion, and apply the base inequality.