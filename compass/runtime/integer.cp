needle 2007;

"**
 * Basic definition of integers (whole numbers) positive and negative,
 * and the messages they implement.
 *
 * Integers is meant here in the most plain and practical sense.
 *
 * @author Daniel Perelman (dap56@cornell.edu)
 * @since needle 2007
 *"
"package compass::number::integer;"
"import compass::comparable;"

"**
 * Contains common integer messages.
 *
 * @mutability[immutable-compatible]
 *"
public mixin $Integer;
$Integer mixin $Comparable;
$Integer mixin $Number;

"**
 * Compute my opposite.
 *
 * This is equivalent to multiplying by negative one.
 *
 * @receiver[7,ro] the original integer
 * @return[only] $Integer: the opposite
 *"
public $Integer abstract #negate;

"**
 * Compute the sum of two integers.
 *
 * Addition is commutative.
 *
 * @receiver[7,ro] the augend
 * @param[7,ro restrict] $Integer: the addend
 * @return[only] $Integer: the sum 
 *"
"public $Integer abstract #+;"

"**
 * Compute the difference between two integers.
 *
 * a-b+b equals a unless the bounds of the type is reached.
 *
 * @receiver[7,ro] the minuend
 * @param[7,ro restrict] $Integer: the subtrahend
 * @return[only] $Integer: the difference
 * @see #+
 *"
"public $Integer abstract #-;"

"**
 * Compute the product of two integers.
 *
 * @receiver[7,ro] the first factor
 * @param[7,ro restrict] $Integer: the second factor
 * @return[only] $Integer: the product
 *"
"public $Integer abstract #*;"

"**
 * Compute integer division.
 *
 * This is integer division, so the remainder is thrown away.
 *
 * a/b*b equals a if a is divisible by b.
 *
 * @receiver[7,ro] the dividend
 * @param[7,ro restrict] $Integer: the divisor
 * @return[only] $Integer: the quotient
 * @see #%
 *"
public $Integer abstract #\;

"**
 * Compute the remainder of integer division.
 *
 * <code>a%b=a-(a/b*b)</code>
 *
 * @receiver[7,ro] the dividend
 * @param[7,ro restrict] $Integer: the divisor
 * @return[only] $Integer: the remainder; has the same sign as the dividend
 * TODO sign of remainder? undefined?
 * @see #\
 *"
"public $Integer abstract #%;"

"**
 * Compute integer exponentiation.
 *
 * @receiver[7,ro] the base
 * @param[7,ro restrict] $Integer: the exponent
 * @return[only] $Integer: the result of the exponentiation or the power
 *"
public $Integer abstract #**;
"friend compass::number::integer!!exponentiation;"

"**
 * Ask if I am divisible by another integer.
 *
 * @param[7,ro restrict] other $Integer
 * @return[global] $Boolean: if I am
 * @see #divides:
 *"
public $Integer divisibleBy: @other {
    ^%@other=0
}
"**
 * Ask if I divide another integer.
 *
 * @param[7,ro restrict] other $Integer
 * @return[global] $Boolean: if I do
 * @see #divisibleBy:
 *"
public $Integer divides: @other {
    ^@other divisibleBy: self
}

"** Ask if I am even. That is to say: ask if I am divisible by two.
 *
 * @return[global] $Boolean: if I am even
 *"
public $Integer isEven {
    ^divisibleBy: 2
}

"** Ask if I am odd. That is to say: ask if one plus me is divisible by two.
 *
 * @return[global] $Boolean: if I am odd
 *"
public $Integer isOdd {
    ^isEven not
}

"TODO [Jimmy] style"
"TODO doc"
public $Integer min: @other {
    <@other || {^@other}
}
public $Integer max: @other {
    >@other || {^@other}
}

$Integer asString {
    isNegative and: { ^'-'.negate asString };
    > 9 ? {
	^(\ 10) asString . ($AsciiCharacter createDigit: (basicMod: 10))
    } // {
	^$AsciiCharacter createDigit: self
    }
}
