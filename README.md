# fairytale
Extendable and fast script language

### download latest version
CI builds are WIP

### how to build
WIP, use cmake. I tried to only build it for windows with msvcpp19, but there is no platform-dependant code (it has no third-party dependencies at all)

### how lo launch
use `./fairytale` in build directory to run interactive mode or pass script filename as first argument to evaluate the file

### Examples
#### Math, lambdas, tests, modules
```
test = import_module("test_module.ft");

#ok, temorary we need this hack, because this name should be global in main global dict
test_number = test.test_number;

test.assert_eq(2+pow(2,1*3)+2*3, 16)

sum = func(a, b)
{
	return a+b;
}

getSum = func()
{
	return sum;
}

a = getSum()(10, 15);
test.assert_eq(a, 25);

obj.getTestObj = func()
{
	testObj.sum = func(a, b)
	{
		return a+b;
	}
	return testObj;
}

test.assert_eq(obj.getTestObj().sum(10, 25), 35);
```

#### OOP
```

animal = func(size)
{
	self = imported_module.object_with_name("animal")
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
cat.print_name()
```
