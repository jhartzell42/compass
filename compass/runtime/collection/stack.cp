needle 2007;

"TODO doc"
"TODO do these make sense for $Collection?"
"package compass::collection::stack;"

public mixin $Stack;
$Stack mixin $Collection;

"**
 * Remove my last element. The last element is the element with the highest
 * index.
 *
 * @return[reference] my former last element
 *"
public $Stack abstract #pop;

"TODO doc"
public $Stack abstract #peek;

"**
 * Mutate me so that I have all my previous elements, in addition to this one
 * new element. This element will have the lowest possible index higher
 * than all other indices.
 *
 * @param[reference] the new element
 * @see #add:
 *"
public $Stack abstract #push:;
