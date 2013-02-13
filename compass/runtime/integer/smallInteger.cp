needle 2007;

"**
 * Most basic integer implementation.
 *"
"package compass::number::integer!!smallInteger;"
"import compass::number::integer;"
"import compass::comparable::adaptor;"

"**
 * Low-level integer with processor implemented operations.
 *
 * This will return an arbitary-precision integer in case an operation
 * would otherwise result in an overflow/underflow.
 *"
private class $SmallInteger;
$SmallInteger mixin $Integer;

$SmallInteger class #new {
    ^0
}
$SmallInteger #negate #SmallIntegerNegate;
$SmallInteger #basicPlus: #SmallIntegerAdd;
$SmallInteger #basicMinus: #SmallIntegerSub;
$SmallInteger #basicTimes: #SmallIntegerMul;
public $SmallInteger #\ #SmallIntegerDiv;
$SmallInteger #basicDivide: @someInt {
    ^(createRational: @someInt) reduce
}
$SmallInteger #basicMod: #SmallIntegerMod;

$SmallInteger = @other {
    ^@other isSmallInteger?{
        ==@other
    }//{
        @other=self
    }
}

$SmallInteger mixin $ComparableLessGreaterAdaptor;
$SmallInteger #basicLess: #SmallIntegerLess;
$SmallInteger #basicGreater: #SmallIntegerGreater;

public #adaptInteger:;

$SmallInteger as: @someClass {
    ^@someClass adaptInteger: self
}

public $Object isSmallInteger {
    ^$false
}

$SmallInteger isSmallInteger {
    ^$true
}
