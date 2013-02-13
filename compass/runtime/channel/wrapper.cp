needle 2007;

"package compass::channel::wrapper;"

public mixin $ChannelWrapper;

public $ChannelWrapper class abstract #newFromChannel:;

public $Channel wrap: @wrappers {
    @res := self;
    @wrappers forwardsLoop: { @wrapper --
	@res := @wrapper newFromChannel: @res
    };
    ^@res
}
