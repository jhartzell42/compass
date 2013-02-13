needle 2007;

"**
 * A hash function generates an integer from an object. Ideally, the hash value
 * is very different for two objects even with only slight differences.
 *
 * TODO doc?
 *"
"package compass::feature::hash;"
"import compass::feature;"
"import compass::feature::equivalenceRelation;"

public mixin $HashFunction;
$HashFunction mixin $EquivalenceRelation;

"**
 * Ask for the hash value of an object according to me.
 *
 * @receiver[7,ro]
 * @param[restrict] the object
 * @return[7,ro only] $SmallInteger: the hash value
 * @return $UncharacterizableException
 *"
public $HashFunction abstract #hash:;

"**
 * Ask for the hash value of an object according to me below some upper bound.
 *
 * @receiver[7,ro]
 * @param[restrict] the object
 * @param[7,ro restrict] $Integer: the upper bound
 * @return[7,ro only] $SmallInteger: the hash value less than the upper bound
 * @return $UncharacterizableException
 *"
public $HashFunction hash: @obj below: @bound {
    ^(hash: @obj) % @bound
}

"**
 * Ask for my identity hash value.
 * An identity hash is a hash value based on which object I am as opposed to
 * what my value is.
 *
 * @receiver[7,ro]
 * @return[7,ro only] $SmallInteger: the identity hash value
 *"
public $Object final #identityHash #ObjectIdentityHash;

"**
 * Ask for my hash value according to my default hash funtion.
 * If I compare equal to another object then we should have the same hash
 * value.
 *
 * @receiver[7,ro]
 * @return[7,ro only] $SmallInteger: the identity hash value
 *"
public $Object hash {
    ^identityHash
}

"**
 * Convert me into a hash function.
 *
 * @return[only] $HashFunction: resultant hash function
 *"
public $HashFunction asHashFunction;

private class $BlockHashFunction($BlockWrapper);
$BlockHashFunction mixin $HashFunction;

"**
 * @receiver[7,ro] a hash function $Block
 * @blockparam[7,ro restrict] the object to hash
 * @blockreturn[global] $SmallInteger: the hash value for the object
 *"
$Block asHashFunction {
    $InvalidConversionException assert: (1 = argumentCount);
    ^$BlockEquivalenceRelation newWithBlock: self
}

$BlockEquivalenceRelation hash: @obj {
    ^block(@obj)
}

"** Hash function representing the default hash function of objects. *"
public global $defaultHashFunction = #hash asHashFunction;

"** Hash function representing the identity hash function of objects. *"
public global $identityHashFunction = #identityHash asHashFunction;
