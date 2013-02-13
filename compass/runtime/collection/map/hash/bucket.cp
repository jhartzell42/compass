needle 2007;

"TODO doc"
"package compass::collection::map!!hash!!bucket"

public class $BucketHashMap($HashMap);

private singleton $emptyBucket;
$emptyBucket mixin $List; " TODO empty list mixin? "

public $BucketHashMap emptySpace {
    ^$emptyBucket
}
"** hash: == indexOf: because we are using buckets, not probing. *"
public $BucketHashMap indexOf: @key (1,ro) {
    ^hash: key
}
private $BucketHashMap bucket: @key {
    ^array[indexOf: @key]
}

public $BucketHashMap [ @key ] {
    @bucket := bucket: @key;
    @bucket search: {} " TODO search/find method? "
}
