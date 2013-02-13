"**
 * Contains common exceptions applicable to all realms of compass.
 *"
"package compass::exception::common;"

"**
 * A message was sent to an object which did not know what to do with it.
 *
 * @see $UnsupportedOperationException
 *"
"TODO Should have field with selector at least... the other exceptions here
      could probably do to have some fields as well."
exception $MessageNotUnderstoodException
    #onMessageNotUnderstood;

"**
 * An argument passed was of a bad type.
 *"
exception $InvalidArgumentException
    #onInvalidArgument;

"**
 * An undefined conversion was attempted.
 *"
exception $InvalidConversionException
    #onInvalidConversion;

"**
 * This method is semantically coherent at some level but not supported.
 *
 * @see $MessageNotUnderstoodException
 *"
exception $UnsupportedOperationException
    #onUnsupportedOperation;
