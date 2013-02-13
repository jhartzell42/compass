needle 2007;

"**
 * Basic low-level object methods and messages.
 *"
"package compass::object;"
"import compass::exception::common;"

"**
 * Root class of the hierarchy.
 *
 * Contains all obligatorily implemented messages and a default implementation
 * for each.
 *"
public class $Object;
public $Object #printMethodTableName #SimpleKeyboardIOPrintMethodTableName;

"**
 * Ask if I am equivalent to this other object, for my definition
 * of equivalent.
 *
 * @param[7,ro restrict] other object
 * @return[global] $Boolean: if I am equivalent.
 *"
public $Object alias #= #==; "default to reference-equivalence"

public $Object != @other {
    ^(= @other) not
}

    
"**
 * Ask if I am reference-equal to this other object, ignoring securelevel
 * information?
 *
 * @param[7,ro restrict] other object 
 * @return[global] $Boolean: if I am equal.
 *"
public $Object final #== #ReferenceCompare;

public $Object final !== @other {
    ^(==@other) not
}

"**
 * Ask if I am reference-equal to this other object, and have the same
 * securelevel and writability?
 * 
 * @param[restrict,reference] the other reference
 *"
public $Object final #=== #SecurityReferenceCompare;

"**
 * Ask for me myself.
 * @return[self]
 *"
public $Object final self;

"**
 * Make a new one of me.
 *"
public $Object constructor new (1,ro);

"**
 * Make me ready for use, but do not secure me.
 *"
public $Object initialize (0,rw);

"**
 * Make me secure, by making a new reference with a securelevel of 1,
 * and with freezing applied, if applicable.
 *
 * @return[self] Self with security
 *"
public $Object secure (0,rw); "{
    ^secured: 1;
}"

"**
 * Ask if I belong to a class or mixin.
 *
 * @param[7,ro restrict] $Type: type to belong to.
 * @return[global] $Boolean: if I belong
 *"
public $Object isa: @type {
    ^(@type in: class mixins) && {@type in: class superclasses};
}

"**
 * Ask what class I am.
 *
 * Primitive method automatically filled in by the compiler.
 * @return[global] $Object class: class I am in.
 *"
"public $Object class;"

"**
 * Ask which type I must saliently belong to.
 *
 * @return[global] $Type: type I belong to
 *"
public $Object species {
    ^class
}

public $Object messageNotUnderstood: _ {
    $MessageNotUnderstoodException()
}
