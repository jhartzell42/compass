needle 2007;

"**
 * Methods and mixins dealing with collections of elements, with various
 * forms of metadata attached and constraints placed upon the elements.
 *"
"package compass::collection;"

"**
 * Parent of all collections.
 * TODO: do iterators.
 * TODO: fix formalised aliasing tags so they are coherent.
 *"
public mixin $Collection;

"**
 * Ask for the default factory to use for making new collections using this
 * collection as a starting point.
 *"
$Collection #species;
