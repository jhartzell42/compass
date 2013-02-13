needle 2007;

"TODO does this have more to do with containment or equivalence or should the
    two be in separate packages or...?"
"package compass::collection::equivalence;"

"**
 * Ask if a reference is one of my elements.
 * 
 * @param[reference] the reference
 * @return[global] $Boolean: if it is
 *"
public $Collection contains: @el (7,ro) {
    ^(reduce: { @bool, @item --
	@bool || {is: @el equalTo: @item}
    } withIdentity: $false)
}

public $Collection alias #has: #contains:;

"**
 * Ask if an object is equal to another object for my purposes.
 *
 * @param one object to compare
 * @param other object to compare
 * @return[global] $Boolean: if it is.
 *"
"TODO We probably want some sort of equality comparator/hasher type like
      we already have $Ordering."
public $Collection is: @a equalTo: @b (1,ro) {
    ^equivalenceRelation is: @a equalTo: @b
}
"TODO doc"
public $Collection equivalenceRelation (1,ro) {
    ^$defaultEquivalenceRelation
}
"**
 * Ask whether I am an element of a collection.
 *
 * @param[7,ro restrict] $Collection: the collection
 * @return[global] $Boolean: if I am.
 *"
public $Object final in: @collection {
    ^@collection contains: self;
}

"**
 * Ask whether I am in a collection.
 *
 * @param[7,ro restrict] $Collection: the collection to ask about
 * @return[global] $Boolean: whether I am *not* in that collection.
 *"
public $Object final notIn: @collection {
    ^(in: @collection) not
}
