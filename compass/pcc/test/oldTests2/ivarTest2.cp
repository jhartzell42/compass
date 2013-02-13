needle 2007;
class $Object ($Object);
class $Blah ($Object) !val1,!val2;
class $Blah2 ($Blah) !val2;
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

$Blah2 #setVal: @val {
    !val2 := @val
}

$Blah2 #val {
    ^!val2
}

$Blah #clear {
    !val2 := 371;
}

$Object class #main {
    @foo := $Blah2 new;
    {
	@foo setVal: $IO readChar;
	@foo clear;
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

class $IO($Object);

$IO class #readInt #SimpleKeyboardIOReadInt;
$IO class #readChar #SimpleKeyboardIOReadChar;
$SmallInteger #printInt #SimpleKeyboardIOPrintInt;
$SmallInteger #printChar #SimpleKeyboardIOPrintChar;

class $Block($Object);

$Block #evaluate #BlockEvaluate;
$Block #() #BlockEvaluate;

class $Boolean($Object);
class $true($Boolean);
class $false($Boolean);

$Boolean #new {
    "what the frik are you doing? booleans aren't classes really, they're
    singletons. Of course, we could use some singleton support."
    exceptionsGalore
}

$true class #?// @trueBlock, @falseBlock {
    ^@trueBlock()
}

$false class #?// @trueBlock, @falseBlock {
    ^@falseBlock()
}

$Object #true {
    ^$true
}

$Object #false {
    ^$false
}

class $Nil($Object);
$Nil class #isNil {
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
