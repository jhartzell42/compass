needle 2007;

"package compass::channel;"
public mixin $Channel;

public mixin $InputChannel;
$InputChannel mixin $Channel;

public mixin $OutputChannel;
$OutputChannel mixin $Channel;

public $InputChannel abstract #next;
public $OutputChannel abstract #put:;

public $InputChannel -> @blk {
    { 
	{
	    @blk(next)
	} repeat
    } onEndOfChannel: {
    }
}

public exception $EndOfChannelException #onEndOfChannel:;
