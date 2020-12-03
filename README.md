# fairytale
Extendable and fast script language

### how to build
WIP, use cmake

### how lo launch
use './fairytale' in build directory to run interactive mode or pass script filename as first argument to evaluate the file

### Examples
'''
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
	testObj = 1;
	testObj.sum = func(a, b)
	{
		return a+b;
	}
	return testObj;
}
a = obj.getTestObj().sum(10, 25);
test.assert_eq(a, 35);
'''

