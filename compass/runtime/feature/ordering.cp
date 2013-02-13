needle 2007;

"**
 * Support for custom orderings of objects.
 *"
"package compass::feature::ordering;"
"import compass::comparable;"
"import compass::feature;"
"import compass::feature::equivalenceRelation;"

"**
 * Implementation of a custom ordering, or, more concretely, an encapsulation
 * of a consistent comparator method.
 *"
public mixin $Ordering;
$Ordering mixin $EquivalenceRelation;

"**
 * Compare two objects using the ordering I encapsulate.
 *
 * This method is implemented consistently. Two equal elements are always
 * be equal, and two non-equal elements always compare consistently.
 * If <code>compare: a to: b</code> returns <code>$less</code>, then
 * <code>compare: b to: a</code> must return <code>$greater</code>.
 *
 * @receiver[7,ro]
 * @param[user,restrict] the left operand
 * @param[user,restrict] the right operand
 * @return[global] $ComparisonResult: the relation of the left operand to
 *                                    the right operand - $less if the
 *				      left operand is less, etc.
 * @throws $UncharacterizableException
 *"
public $Ordering abstract #compare:to:;

$Ordering abstract is: @a equalTo: @b {
    ^(compare: @a to: @b) == $equal
}

"**
 * Convert me into an ordering.
 *
 * @return[only] $Ordering: resultant ordering
 *"
public $Ordering asOrdering;

private class $BlockOrdering($BlockWrapper);
$BlockOrdering mixin $Ordering;

"**
 * @receiver[7,ro] an ordering $Block
 * @blockparam[7,ro restrict] first element
 * @blockparam[7,ro restrict] second element
 * @blockreturn[global] $ComparisonResult: $less if first element is less, etc.
 *"
$Block asOrdering {
    $InvalidConversionException assert: (2 = argumentCount);
    ^$BlockOrdering newWithBlock: self
}

$BlockOrdering compare: @a to: @b {
    ^block(@a,@b)
}

"** Ordering representing default ordering of objects. *"
public global $defaultOrdering = #<=> asOrdering;
