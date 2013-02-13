needle 2007;
class $Object ($Object);
$Object messageNotUnderstood: @a {
    infiniLoop "ha ha what cruelty. this segfaults."
}

$Object class new {
    ^basicNew
}

$Object class main {
    @temp := ($IO readInt); 
    { @foo -- (@foo+@temp)printInt;} (@temp);
    @foo printInt;
    $true print;
    $false print;
    ^@foo
}

class $SmallInteger($Object);

$SmallInteger #+ #SmallIntegerAdd;
$SmallInteger #- #SmallIntegerSub;
$SmallInteger #* #SmallIntegerMul;
$SmallInteger #/ #SmallIntegerDiv;
$SmallInteger #negate #SmallIntegerNegate;

$SmallInteger class new {
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

mixin $Boolean #isBoolean;
$Boolean print {
    (? {84} // {70}) printChar;
    10 printChar;
}

singleton $true($Object);
$true mixin $Boolean;

singleton $false($Object);
$false mixin $Boolean;

$true ? @trueBlock // _ {
    ^@trueBlock()
}

$false ? _ //@falseBlock {
    ^@falseBlock()
}

$Object true {
    ^$true
}

$Object false {
    ^$false
}

singleton $Nil($Object);
$Nil isNil {
    ^true
}

$Object isNil {
    ^false
}

$Block -> @otherBlock {
    ()?{
	@otherBlock();
	-> @otherBlock;
    }//{
	^
    }
}
