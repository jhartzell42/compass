needle 2007;
class $Object ($Object);
class $Blah ($Object) !val1,!val2;
$Object #messageNotUnderstood: @a {
    forget
}

$Object class #new {
    ^basicNew initialize
}

$Object #initialize {
}

$Blah #initialize {
    !val2 := 371
}

$Blah #setVal: @val {
    !val1 := @val
}

$Blah #val {
    ^!val1
}

$Object class #main {
    @foo := $Blah new;
    {
	@foo setVal: $IO readChar;
	@foo val isNil ? {
	    false
	} // {
	    true
	}
    } -> {
	@foo val printChar
    }
}

class $SmallInteger($Object);

$SmallInteger #+ #SmallIntegerAdd;
$SmallInteger #- #SmallIntegerSub;
$SmallInteger #* #SmallIntegerMul;
$SmallInteger #/ #SmallIntegerDiv;
$SmallInteger #negate #SmallIntegerNegate;

$SmallInteger class #new {
    ^0
}

singleton $IO($Object);

$IO #readInt #SimpleKeyboardIOReadInt;
$IO #readChar #SimpleKeyboardIOReadChar;
$SmallInteger #printInt #SimpleKeyboardIOPrintInt;
$SmallInteger #printChar #SimpleKeyboardIOPrintChar;

class $Block($Object);

$Block #evaluate #BlockEvaluate;
$Block #() #BlockEvaluate;

class $Boolean($Object);
singleton $true($Boolean);
singleton $false($Boolean);

$Boolean #new {
    "what the frik are you doing? booleans aren't classes really, they're
    singletons. Of course, we could use some singleton support."
    exceptionsGalore
}

$true #?// @trueBlock, @falseBlock {
    ^@trueBlock()
}

$false #?// @trueBlock, @falseBlock {
    ^@falseBlock()
}

$Object #true {
    ^$true
}

$Object #false {
    ^$false
}

singleton $Nil($Object);
$Nil #isNil {
    ^true
}

$Object #isNil {
    ^false
}

$Block #-> @otherBlock {
    ()?{
	@otherBlock();
	-> @otherBlock;
    }//{
	^
    }
}
