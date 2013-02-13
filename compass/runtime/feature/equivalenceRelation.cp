needle 2007;

"**
 * Support for custom equality between objects.
 *"
"package compass::feature::equivalenceRelation;"

"**
 * Implementation of a custom equivalence relation, or, more concretely,
 * an encapsulation of a consistent equality checking method.
 *"
public mixin $EquivalenceRelation;
$EquivalenceRelation mixin $Characterization;

"**
 * Compare two objects using the equivalence relation I encapsulate.
 *
 * @receiver[7,ro]
 * @param[user,restrict] the left operand
 * @param[user,restrict] the right operand
 * @return[global] $Boolean: if the two operands are equal according to me
 * @throws $UncharacterizableException
 *"
public $EquivalenceRelation abstract #is:equalTo:;

"**
 * Convert me into an equivalence relation.
 *
 * @return[only] $EquivalenceRelation: resultant equivalence relation
 *"
public $EquivalenceRelation asEquivalenceRelation;

private class $BlockEquivalenceRelation($BlockWrapper);
$BlockEquivalenceRelation mixin $EquivalenceRelation;

"**
 * @receiver[7,ro] an equivalence relation $Block
 * @blockparam[7,ro restrict] first element
 * @blockparam[7,ro restrict] second element
 * @blockreturn[global] $Boolean: if the elements are equal
 *"
$Block asEquivalenceRelation {
    $InvalidConversionException assert: (2 = argumentCount);
    ^$BlockEquivalenceRelation newWithBlock: self
}

$BlockEquivalenceRelation is: @a equalTo: @b {
    ^block(@a,@b)
}

"** Equivalence relation representing default equivalence of objects. *"
public global $defaultEquivalenceRelation = #= asEquivalenceRelation;

"**
 * Equivalence relation representing identity equivalence of references.
 *
 * @see #==;
 *"
public global $identityEquivalenceRelation = #== asEquivalenceRelation;

"**
 * Equivalence relation representing identity, securelevel, and writability
 * equivalence of references.
 *
 * @see #===;
 *"
public global $exactEquivalenceRelation = #=== asEquivalenceRelation;
