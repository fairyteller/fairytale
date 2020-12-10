# fairytale
Extendable and fast script language

### download latest version
CI builds are WIP

### how to build
WIP, use cmake. I tried to only build it for windows with msvcpp19, but there is no platform-dependant code (it has no third-party dependencies at all)

### how lo launch
use `./fairytale` in build directory to run interactive mode or pass script filename as first argument to evaluate the file

### Roadmap
* Catch specific types of exceptions
* Variadic arguments
* File IO built-in module
* Select the names to import from module, import to global namespace
* Dictionaries
* List comprehension and range based for
* Strong Typing
* Full support for classes
* Default arguments, named parameters
* Decorators
* Overloading
* Operator overloading

### Current state examples
(Github extension for Fairytale syntax highlighting is WIP)

#### Math, lambdas, tests, modules
```
test = import_module("test_module.ft");

test.assert_eq(true, true);

a = 3
test.assert_eq(-2+pow(2,1*a)+2*a, 12)

test.assert_eq(-2* -2, 4)

sum = func(a, b)
{
	return a+b;
}

test.assert_eq(sum(5, 10), 15);

getSum = func()
{
	return self.sum; #TODO: fix self. !
}

a = getSum()(10, 15);
test.assert_eq(a, 25);

obj = object();
obj.getTestObj = func()
{
	testObj = object();
	testObj.sum = func(a, b)
	{
		return a+b;
	}
	return testObj;
}

test.assert_eq(obj.getTestObj().sum(10, 25), 35);

b = array(1, 2, 3, 4, 5);
test.assert_eq(b.size(), 5);

test.assert_eq(b[2], 3);

b = array(b.unroll());
test.assert_eq(b[2], 3);

b[3] = 10;
test.assert_eq(b[3], 10);

a = "init";
try
{
	a = "ok";
}
catch
{
	a = "catched";
}
test.assert_eq(a, "ok");

a = "init";
try
{
	a = "ok";
	throw "just a string"
}
catch
{
	a = "catched";
}
test.assert_eq(a, "catched");

a = "init";
try
{
	throw "just a string"
	a = "should not be called";
}
catch
{}
test.assert_eq(a, "init");
```

#### OOP-like code (proper classes are in roadmap after strong typing)
```

animal = func(size)
{
	self = object()
	self.size = size
	self.legs = 0
	self.walk = func()
	{
		print("I have size " + str(self.size) + " and walk on " + str(self.legs) + " legs")
	}
	return self;
}


human = func()
{
	self = animal(4)
	self.legs = 2
	self.playSound = func()
	{
		print("Hello, it is a sane language")
	}
	return self;
}

cat = func()
{
	self = animal(2)
	self.legs = 4
	self.playSound = func()
	{
		print("Meow")
	}
	return self;
}

person = human()
cat = cat()
person.walk()
person.playSound()
cat.walk()
cat.playSound()
```
