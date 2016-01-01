
function setupGraph(root, options) {
    root = $(root);

    var root_node = root.get(0);
    var margin = options.margins || {top: 20.5, right: 30, bottom: 30, left: 40.5};
    var width = root.width() - margin.left - margin.right;
    var height = root.height() - margin.top - margin.bottom;

    var xScale = d3.scale.linear()
            .range([0, width]);

    var yScale = d3.scale.linear()
            .range([height, 0]);

    var xAxis = d3.svg.axis()
            .scale(xScale)
            .orient("bottom");

    var yAxis = d3.svg.axis()
            .scale(yScale)
            .orient("left");

    var svg = d3.select(root_node).append("svg")
            .attr("width", width + margin.left + margin.right)
            .attr("height", height + margin.top + margin.bottom)
            .append("g")
            .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

    var data = [];
    var xAxisNode = svg.append("g")
            .attr("class", "x axis")
            .attr("transform", "translate(0," + height + ")");

    var yAxisNode = svg.append("g")
            .attr("class", "y axis");

    yAxisNode.call(yAxis)
            .append("text")
            .attr("class", "title")
            .attr("transform", "rotate(-90)")
            .attr("y", 6)
            .attr("dy", ".71em")
            .text("Value");

    var lines = [];
    var line_bus = new Bacon.Bus();
    var line_id = 0;
    var limit = options.limit || null;
    var limit_bus = new Bacon.Bus();
    var limited = limit_bus.toProperty(limit !== null);

    graph = {
        dataCallback : function (entry) {
            var self = this;
            data.push(entry);
            if(data.length < 2) {
                return;
            }
            if(limit !== null) {
                data = data.slice(-limit);
            }
            var xextents = [];
            var yextents = [];
            _.each(lines, function(line) {
                if(!line._enabled) {
                    return;
                }
                xextents = xextents.concat(d3.extent(data, line.xf));
                yextents = yextents.concat(d3.extent(data, line.yf));
                if(line.config.global_minmax && line.config.first_data) {
                    yextents = yextents.concat(yScale.domain());
                }
                line.config.first_data = true;
            });
            xScale.domain(d3.extent(xextents));
            yScale.domain(d3.extent(yextents));
            xAxisNode.call(xAxis);
            yAxisNode.call(yAxis);
            _.each(lines, function(line) {
                var lsvg = svg.select("#" + line.id);
                if(line._enabled) {
                    // change the line
                    lsvg.attr("d", line.line(data));
                } else {
                    lsvg.attr("d", "");
                }
            });
        },

        line : function(xf, yf, config) {
            config = _.extend({
                color : "#000000",
                global_minmax : false,
                name: "unknown"
            }, config || {});

            var color = config.color ;
            var self = this;
            var id = "line-" + line_id++;

            var line = d3.svg.line()
                    .x(function(d) { return xScale(xf(d)); })
                    .y(function(d) { return yScale(yf(d)); });

            svg.append("path")
                .attr("class", "line")
                .attr("style", "stroke: " + color)
                .attr("id", id)
                .datum(data)
                .attr("d", line);

            lines.push({
                xf : xf,
                yf : yf,
                line : line,
                id : id,
                _enabled: true,
                config : config,
                enable : function(enable) {
                    if(enable !== undefined) {
                        this._enabled = enable;
                        line_bus.push(this);
                    }
                    return this._enabled;
                }
            });
            line_bus.push(lines[lines.length -1]);
            return self;
        },

        clear: function() {
            data = [];
            _.each(lines, function(line) {
                line.config.first_data = false;
            });
        },

        limit: function(new_limit, toggle) {
            if(toggle !== undefined && toggle && new_limit == limit) {
                limit = null;
            } else {
                limit = new_limit;
            }
            limit_bus.push(limit !== null);
        },
        limited: limited,
        lines : line_bus
    };
    _.bindAll(graph, "dataCallback", "line", "clear", "limit");
    return graph;
}
