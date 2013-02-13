needle 2007;

"**
 * Support for comparing orderable objects.
 *
 * This allows a type to set a default ordering on its objects by
 * implementing ordering methods. For every two items of that type, either
 * one item must be consistently greater than the other, or the items must
 * be consistently equivalent in some way.
 *
 * Note that <@link=#=> is defined for all objects, and therefore does not
 * appear here. The implementation of these methods, however, must form
 * a coherent ordering along with #=.
 *
 * Other orderings may be encapsulated in a custom ordering object.
 *"
"package compass::comparable;"

"**
 * Contains common messages for objects belonging to types with a default
 * ordering, allowing you to access that default ordering.
 *"
public mixin $Comparable;

"**
 * A comparison was made between two types which are not mutually comparable.
 *"
public exception $UncomparableException ($InvalidArgumentException)
    #onUncomparable;

"**
 * Determine whether I am less than another object.
 *
 * @param[7,ro restrict] $Comparable: the other object
 * @return[global] $Boolean: whether I am less
 * @throws $UncomparableException
 *"
public $Comparable abstract #<;
"**
 * Determine whether I am less than another object.
 *
 * @param[7,ro restrict] $Comparable: the other object
 * @return[global] $Boolean: if I am greater
 * @throws $UncomparableException
 *"
public $Comparable abstract #>;
"**
 * Determine whether I am less than or equal to another object.
 *
 * @param[7,ro restrict] $Comparable: the other object
 * @return[global] $Boolean: if I am either less or equal
 * @throws $UncomparableException
 *"
public $Comparable abstract #<=;
"**
 * Determines whether I am greater than or equal to another object.
 *
 * @param[7,ro restrict] $Comparable: the other object
 * @return[global] $Boolean: if I am either greater or equal
 * @throws $UncomparableException
 *"
public $Comparable abstract #>=;

"**
 * Enumeration of the possible results of a comparison.
 *
 * This allows the result of a comparison to be handled by polymorphism.
 *
 * @see #<=>
 *"
public enum $ComparisonResult
    less: ($less)
    greater: ($greater)
    equal: ($equal);

"**
 * Determine the ordering between me and another object.
 *
 * This operator was inspired by Perl's <q>spaceship operator</q>, which
 * looks and acts similarly.
 *
 * @param[7,ro restrict] $Comparable: the other object
 * @return[global] $ComparisonResult: $less if I am less,
				      $greater if I am greater
				      $equal if I am equal.
 * @see $ComparisonResult
 * @throws $UncomparableException
 *"
public $Comparable abstract #<=>;

public $Comparable between: @a and: @b {
    ^>=@a and: {<=@b}
}
