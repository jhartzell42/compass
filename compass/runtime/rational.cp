needle 2007;

"**
 * Basic definition of rational numbers, which are any numbers which can be
 * written as a fraction of two integers.
 *
 * Any rational number can be written many ways by using different
 * denominators. Rationals may, but are not required to, guarantee that they
 * are in reduced form, which is unique for any given rational number.
 *
 * @author Daniel Perelman (dap56@cornell.edu)
 * @author James Hartzell (jah259@cornell.edu)
 * @since needle 2007
 *"
"package compass::number::rational;"

"**
 * Contains common rational messages.
 *
 * @mutability[immutable-compatible]
 *"
public mixin $Rational;
$Rational mixin $Number;
$Integer mixin $Rational;

"**
 * Ask for my numerator. When a rational number is written as a fraction, the
 * numerator is the part written above the line.
 *
 * @receiver[7,ro]
 * @return[only] $Integer: my numerator
 *"
public $Rational abstract #numerator;

$Integer numerator;

"**
 * Ask for my denominator. When a rational number is written as a fraction,
 * the denominator is the part written below the line.
 *
 * @receiver[7,ro]
 * @return[only] $Integer: my denominator
 *"
public $Rational abstract #denominator;

$Integer denominator {
    ^1
}

"**
 * Create a new rational.
 *
 * @receiver[7,ro] the numerator
 * @param[7,ro saved] the denominator
 * @return[only] $Rational: the result of the division
 *"
public #createRational:;

"**
 * Compute my reduced form. If I am already reduced, this may return myself.
 *
 * @receiver[7,ro]
 * @return[only,self] $Rational: a reduced rational
 *"
public $Rational reduce {
    @gcd := numerator gcd: denominator;
    ^@gcd = denominator ? {
	numerator\@gcd
    } // {
	(numerator\@gcd) createRational: (denominator\@gcd)
    }
}

public $Integer gcd: @other {
    @other = 0 ? {
	^
    } // {
	^@other gcd: % @other
    }
}

$Rational basicPlus: @other {
    ^(numerator*@other denominator + @other numerator*denominator)/
	(denominator*@other denominator)
}

$Rational basicMinus: @other {
    ^basicPlus: @other negate
}

$Rational basicTimes: @other {
    ^(numerator * @other numerator) / (denominator * @other denominator)
}

public $Rational reciprocal {
    ^denominator/numerator
}

$Rational basicDivide: @other {
    ^basicTimes: @other reciprocal
}

$Rational negate {
    ^numerator negate / denominator
}

$Rational asString {
    ^denominator = 1 ? {
	numerator asString
    } // {
	numerator asString . '/' . denominator asString
    }
}
