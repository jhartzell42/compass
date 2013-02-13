needle 2007;

"TODO doc"
"package compass::collection::functional::setOperation;"

"TODO [Dan] reorder methods sensibly"

"TODO doc"
public $Collection adding: @val {
    ^.[@val,]
}

"**
 * Ask for a new collection such that it does not contain a certain element.
 *
 * Note that different collections may have different definitions of
 * containing that element based on different definitions of equality.
 *
 * @param[reference] the element
 * @return[only] $Collection: the new collection
 *"
public $Collection removing: @val {
    ^|-> { @item --
	(is: @item equalTo: @val) not
    }
}

"TODO doc -- related closely to concat (#.)"
public $Collection abstract #union:; "TODO implement"

"**
 * Ask for a collection of a nature similar to me, with the
 * elements both of mine and another collection's.
 *
 * If some items from the other collection cannot be included due to a
 * constraint, then they are not included.
 *
 * TODO: when writing user-ordered collections, put a refinement in here about
 * how the receiver's elements come first.
 * @param[7,ro restrict] $Collection: other collection
 * @return[only] requested combined collection
 *"
public $Collection . @other {
    "(^species new) {addAll: @@; addAll: @other;}"
    ^@res := species new;
    @res addAll: self;
    @res addAll: @other
}

"**
 * Ask for a collection of my species which contains any elements both in
 * me and another collection, and no others.
 *
 * @param[7,ro restrict] $Collection: the other collection
 * @return[only] $Collection: the new collection
 *"
public $Collection intersect: @other {
    ^ |-> { @@ --
	in: @other
    }
}

"**
 * Create a new collection of my species which contains all of my elements
 * except for those contained in a certain other collection. 
 *
 * @param[7,ro restrict] the other $Collection instance
 * @see #remove:
 * @return[only] new $Collection instance
 *"
public $Collection removingAll: @other {
    |-> { @@ --
	notIn: @other
    }
}

