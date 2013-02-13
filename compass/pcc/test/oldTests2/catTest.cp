needle 2007;
class $Object ($Object);
$Object #messageNotUnderstood: @a {
    forget
}

$Object class #new {
    ^basicNew
}

$Object class #main {
    { (@foo:=($IO readChar))isNil?{false}//{true} }->{@foo printChar}
    "@temp := ($IO readInt); 
    { @foo -- (@foo+@temp)printInt;} (@temp);
    ^4"
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
