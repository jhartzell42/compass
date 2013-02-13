needle 2007;

"TODO doc"
"** Basic messages for changing what items are in a collection. *"
"package compass::collection::update;"

"**
 * Mutate me so that I also contain the item specified.
 *
 * @param[reference] item to add.
 *"
public $Collection abstract #add:;

"**
 * Mutate me so that I no longer contain the item specified.
 *
 * Note that no longer containing the item specified might include not
 * including any items that compare equal to it with #=, #==, #===, or a
 * custom comparator.
 *
 * @param[reference] item to remove
 * @throws $NoSuchElementException
 * @return[reference] item removed
 *"
public $Collection abstract #remove:;

"**
 * Mutate me so I no longer contain any elements.
 *"
public $Collection abstract #clear;

"*
 * Mutate me so I no longer contain any elements not in another collection.
 *
 * @param[7,ro restrict] the other $Collection
 *"
public $Collection abstract #retainAll:;

"**
 * Mutate me so I no longer contain any elements that are in a certain other
 * collection.
 *
 * @param[7,ro restrict] the other $Collection instance
 * @see #remove:
 *"
public $Collection removeAll: @other {
    @other -> { @elem --
        remove: @elem
    }
}

"**
 * Mutate me so I also contain all the elements contained in another
 * collection.
 *
 * @param[7,ro restrict] $Collection: the other collection
 *"
public $Collection addAll: @other {
    @other -> { @elem --
        add: @elem
    }
}
