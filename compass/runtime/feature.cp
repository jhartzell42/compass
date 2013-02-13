needle 2007;

"**
 * A feature is the abstract idea of some generalizable description
 * of an object. The most basic example is a hash: every object can be
 * described with an integral hash of some sort.
 *
 * The types in this package specify regularized ways for an object called
 * a feature extractor to offer an interpretation of some subset of objects.
 * The code using the feature extractor should only have to know the
 * feature extractor's interface and may be otherwise completely ignorant of
 * the structure of the objects being worked with.
 *
 * In general feature extractors act as a bridge between collections and the
 * objects they contain. Some collections do not require any information about
 * the objects they contain but others need to be able to make comparisions,
 * hash their elements, or perform even more complex operations.
 *
 * TODO This doc reads more like a justification of the concept than a doc...
 *"
"package compass::feature;"

"**
 * A way of looking at some subset of all objects
 * which provides some meaningful information about those objects.
 *
 * For any given reference it either can or cannot characterize that reference.
 * The <@link=#canCharacterize:> message can be used to find out which,
 * although a $Characterization may not be able to tell for sure.
 * If any references the characterization identifies as not characterizable are
 * given to one of its characterization methods, then it must throw
 * an $UncharacterizableException. To clarify, the characterization is allowed
 * to require a certain securelevel on the reference to determine its
 * characterizability.
 *
 * @mutability[immutable]
 *"
public mixin $Characterization;
$Characterization mixin $Immutable;

"**
 * Ask if an object is one that I can characterize. Note that this may return
 * true for object which turns out to be non-characterizable.
 *
 * @receiver[7,ro]
 * @param[restrict] the object
 * @return[global] $Boolean: if it is or if I do not know
 *"
public $Characterization canCharacterize: _ {
    ^$true
}

"** The characterization could not characterize the object given. *"
public exception $UncharacterizableException #onUncharacterizable
    "** The characterization which had the problem. *"
    !characterization,
    "** The object which the characterization had a problem with. *"
    !badObject,
;

public $UncharacterizableException constructor
    newFromCharacterization: !characterization onObject: !badObject;

"**
 * TODO syntax for !foo info shortcuts?
 * @param[saved] {!characterization}
 * @param[saved] {!badObject}
 *"
public $UncharacterizableException class (@args) {
    $InvalidArgumentException assert: @args size >= 2;
    (newFromCharacterization: @args[0] onObject: @args[1]) throw
}

public $UncharacterizableException class undefine #assert;
"**
 * Create and throw a new one of me, if the characterization
 * cannot characterize the object.
 *
 * @param[7,ro saved] the characterization
 * @param[saved] the object
 * @throws $UncharacterizableException
 *"
public $UncharacterizableException class assert: @char canCharacterize: @obj {
    (@char canCharacterize: @obj) or: { (@char, @obj) }
}
"**
 * Throws an exception only if I cannot characterize the object.
 *
 * @receiver[7,ro saved]
 * @param[saved] the object
 * @throws $UncharacterizableException
 *"
public $Characterization assertCharacterizable: @obj {
    $UncharacterizableException assert: self canCharacterize: @obj
}
