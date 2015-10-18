var ZOOM_MAX = 4.0;

function px2int(v) {
    return parseInt(v, 10);
};


var World = {
    loaded: false,

    init: function() {
        this.createOverlays();
    },

    createOverlays: function() {
        var self = this;
        this.tracker = new AR.ClientTracker("assets/jimmy.wtc", {
            onLoaded: this.worldLoaded
        });

        function enter_field_of_vision(target_name) {
                $("#detailButton").show();
                $("#detailButton").click(
                    function() {
                        World.tm.create_grid(imageinfo[target_name].tiles);
                        $("#detailViewDismiss").show();
                        $("#detailView").show();
                        $("#detailBackground").show();
                    });
        };

        function exit_field_of_vision() {
            $("#detailButton").hide();
        };

        // create overlays
        for(var target_name in imageinfo) {
            var target_def = imageinfo[target_name];
            var overlay_data = target_def.overlay;
            var img = new AR.ImageResource(overlay_data.path);
            var overlay = new AR.ImageDrawable(
                img,
                overlay_data.height,
                {
                    offsetX: overlay_data.offset_x,
                    offsetY: overlay_data.offset_y
                }
            );
            var page = new AR.Trackable2DObject(this.tracker, target_name, {
                drawables: {
                    cam: overlay
                },
                onEnterFieldOfVision: enter_field_of_vision,
                onExitFieldOfVision: exit_field_of_vision
            });
        }
    },

    worldLoaded: function() {
        var self = this;
        $("#detailButton").hide();
        $("#detailView").hide();
        $("#detailBackground").hide();
        $("#detailViewDismiss").hide().click(
            function() {
                $("#detailView").hide().empty();
                $("#detailViewDismiss").hide();
                $("#detailBackground").hide();
            });
        World.tm = new TouchManager("#detailView");
    }
};


function TouchManager(target) {
    this._target = $(target);
    _.bindAll(
        this,
        "ontouchstart", "ontouchend", "ontouchmove",
        "pan", "zoom", "create_grid", "zoom_grid",
        "container_position", "viewport2imagecoords"
    );

    this._target.on("touchstart", this.ontouchstart);
    this._target.on("touchend", this.ontouchend);
    this._target.on("touchleave", this.ontouchend);
    this._target.on("touchcancel", this.ontouchend);
    this._target.on("touchmove", this.ontouchmove);
    this._touches = {};
    this._container = null;
    this._can_pan = true;
};


TouchManager.prototype = {
    create_grid: function(tiles) {
        var self = this;
        var root = this._target;
        root.children().remove();
        var container = $("<div class='tile container'/>");
        this._container = container;

        var total_width = 0;
        var total_height = 0;
        _.each(tiles, function(tile_info) {
            var tile = $("<img class='tile'/>");
            tile.attr("src", tile_info.name);
            var coords = tile_info.coords;
            tile.css(
                {
                    "left": coords.left,
                    "top": coords.top,
                    "width": tile_info.width,
                    "height": tile_info.height
                }
            );
            tile.data("info", tile_info);
            tile.appendTo(container);
            total_width = Math.max(total_width, coords.right + 1);
            total_height = Math.max(total_height, coords.bottom + 1);
        });
        container.css(
            {
                left: 0, top: 0,
                width: total_width,
                height: total_height
            }
        );
        root.data("zoom", 1.0);
        root.data("total_width", total_width);
        root.data("total_height", total_height);
        container.appendTo(root);

        var window_width = $(window).width();
        var window_height = $(window).height();
        var img_aspect_ratio = total_width / total_height;
        var view_aspect_ratio = window_width / window_height;

        self._viewport_zoom_center = {
            x : window_width / 2.0,
            y : window_height / 2.0
        };
        self._image_center = {
            x : total_width / 2.0,
            y : total_height / 2.0
        };

        if(img_aspect_ratio >= view_aspect_ratio) {
            // image is broader than the viewport
            self._zoom_min = window_width / total_width;
        } else {
            // image is thinner/taller than the viewport
            self._zoom_min = window_height / total_height;
        }
        self._zoom_max = ZOOM_MAX;

        self.zoom_grid(self._zoom_min);
    },

    zoom_grid: function(new_zoom) {
        var self = this;
        var root = this._target;
        new_zoom = Math.max(Math.min(new_zoom, self._zoom_max), self._zoom_min);
        var total_width = 0;
        var total_height = 0;

        var tiles = self._container.children().get();
        if(new_zoom != 1.0) {
            _.each(
                tiles,
                function(tile) {
                    var info = $(tile).data("info");
                    var width = Math.round(info.width * new_zoom);
                    var height = Math.round(info.height * new_zoom);
                    var left_of_me = info.left_of_me !== null ? $(tiles[info.left_of_me]) : null;
                    var top_of_me = info.top_of_me !== null ? $(tiles[info.top_of_me]) : null;
                    var left = left_of_me !== null ? px2int(left_of_me.css("left")) + px2int(left_of_me.css("width")) : 0;
                    var right = top_of_me !== null ? px2int(top_of_me.css("top")) + px2int(top_of_me.css("height")) : 0;
                    total_width = Math.max(total_width, left + width);
                    total_height = Math.max(total_height, top + height);
                    $(tile).css({
                        "left": left,
                        "top": right,
                        "width": width,
                        "height": height
                    });
                }
            );
        } else {
            _.each(
                tiles,
                function(tile) {
                    var info = $(tile).data("info");
                    var left = info.coords.left;
                    var top = info.coords.top;
                    var width = info.width;
                    var height = info.height;
                    total_width = Math.max(total_width, left + width);
                    total_height = Math.max(total_height, top + height);
                    $(tile).css({
                        "left": left,
                        "top": top,
                        "width": width,
                        "height": height
                    });
                }
            );
        }
        var new_container_pos = {
            x : self._viewport_zoom_center.x - self._image_center.x * new_zoom,
            y : self._viewport_zoom_center.y - self._image_center.y * new_zoom
        };
        console.log(new_container_pos);
        self.container_position(new_container_pos);
        root.data("zoom", new_zoom);
        root.data("total_width", total_width);
        root.data("total_height", total_height);
    },

    container_position : function(new_pos) {
        if(new_pos === undefined) {
            return {
                x : px2int(this._container.css("left")),
                y : px2int(this._container.css("top"))
            };
        } else {
            this._container.css(
                {
                    left : new_pos.x,
                    top : new_pos.y
                }
            );
            return new_pos;
        }
    },

    viewport2imagecoords: function(vp, op, zoom_factor) {
        return {
            x : (vp.x - op.x) / zoom_factor,
            y : (vp.y - op.y) / zoom_factor
        };
    },

    ontouchstart: function(e) {
        var self = this;
        e.preventDefault();
        if(self._container === null) {
            return;
        }

        var event = e.originalEvent;
        _.each(
            event.changedTouches,
            function(touch) {
                self._touches[touch.identifier] = {
                    x : touch.pageX,
                    y : touch.pageY
                };
            }
        );
        var touchnum = _.size(self._touches);
        if(touchnum == 1) {
            self._panpos = self.container_position();
        }
        if(touchnum == 2) {
            self._oldzoom = self._target.data("zoom");
            self._viewport_zoom_center = _.reduce(
                _.values(self._touches),
                function(accu, touch) {
                    accu.x += touch.x / 2.0;
                    accu.y += touch.y / 2.0;
                    return accu;
                },
                { x : 0, y : 0}
            );
            self._image_center = self.viewport2imagecoords(
                self._viewport_zoom_center,
                self.container_position(),
                self._oldzoom
            );
            self._can_pan = false;
        }
    },

    ontouchend: function(e) {
        var self = this;
        e.preventDefault();
        if(self._container === null) {
            return;
        }

        var event = e.originalEvent;
        _.each(
            event.changedTouches,
            function(touch) {
                delete self._touches[touch.identifier];
            }
        );
        var touchnum = _.size(self._touches);
        self._can_pan = touchnum == 0;
    },

    ontouchmove: function(e) {
        var self = this;
        e.preventDefault();
        if(self._container === null) {
            return;
        }

        var event = e.originalEvent;
        var touchnum = _.size(self._touches);

        if(touchnum == 1 && self._can_pan) {
            self.pan(event.changedTouches[0]);
        }  else if(touchnum == 2) {
            self.zoom(event.touches);
        }

    },

    pan: function(touch) {
        var self = this;
        var xd = touch.pageX - self._touches[touch.identifier].x;
        var yd = touch.pageY - self._touches[touch.identifier].y;
        self.container_position(
            {
                x: self._panpos.x + xd,
                y: self._panpos.y + yd
            }
        );
    },

    zoom: function(touches) {
        var self = this;
        var touchpoints = _.values(self._touches);
        var original_length = Math.sqrt(
            Math.pow(touchpoints[0].x - touchpoints[1].x, 2),
            Math.pow(touchpoints[0].y - touchpoints[1].y, 2)
        );
        var current_length = Math.sqrt(
            Math.pow(touches[0].pageX - touches[1].pageX, 2),
            Math.pow(touches[0].pageY - touches[1].pageY, 2)
        );
        var zoomfactor = (current_length / original_length) * self._oldzoom;
        self.zoom_grid(zoomfactor);
    }

};

//World.init();
