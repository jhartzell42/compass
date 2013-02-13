needle 2007;

"**
 * Integer fraction implentation of rational numbers.
 *"
"package compass::number::rational!!fraction;"

public class $IntegerFraction
    !numerator,
    !denominator
;
$IntegerFraction mixin $Rational;

"TODO reduction?"
private $IntegerFraction constructor newWithNumerator: !numerator
    denominator: !denominator;

$Integer createRational: @otherInt {
    ^$IntegerFraction newWithNumerator: self denominator: @otherInt
}

$IntegerFraction class adaptInteger: @int {
    ^@int createRational: 1
}

$IntegerFraction numerator {
    ^!numerator
}

$IntegerFraction denominator {
    ^!denominator
}

$IntegerFraction typeMatches: @other {
    ^@other species = $IntegerFraction or: {
	@other species = $SmallInteger
    }
}
