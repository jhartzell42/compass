needle 2007;

"TODO doc"
"package compass::collection::map;"

public mixin $Map;
$Map mixin $LookupCollection;

"TODO delete from keys/values?"
"**
 * Ask for an object through which you can access my keys.
 *
 * This returns a collection which, when you access it, acts as a collection
 * of all the key parts of my key-value elements. This collection is
 * unwritable, but is connected directly to me and is always in sync with
 * what I presently have for keys. It therefore has, out of necessity, a
 * reference to me.
 * 
 * @return[only] the keys accessor
 *"
public $Map abstract #keys;

"**
 * Ask for an object through which you can access my values.
 *
 * This returns a collection which, when you access it, acts as a collection
 * of all the values parts of my key-value elements. This collection is
 * unwritable, but is connected directly to me and is always in sync with
 * what I presently have for values. It therefore has, out of necessity, a
 * reference to me.
 * 
 * @return[only] the values accessor
 *"
public $Map abstract #values;

"**
 * Ask if a key is equal to another key for my purposes.
 *
 * @param one key to compare
 * @param other key to compare
 * @return[global] $Boolean: if it is.
 *"
"TODO [Jimmy] name?"
public $Map isKey: @a equalTo: @b (1,ro) {
    ^equivalenceRelation is: @a equalTo: @b
}
"TODO doc"
public $Map keyEquivalenceRelation (1,ro) {
    ^$defaultEquivalenceRelation
}
