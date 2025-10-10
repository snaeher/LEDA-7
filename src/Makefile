
all:   x = all
all:   build

obj:   x = obj
obj:   build

clean: x = clean 
clean: build

touch: x = touch
touch: build

build: 
	if [ -d core              ];then $(MAKE) -C core              $(x); fi
	if [ -d system            ];then $(MAKE) -C system            $(x); fi
	if [ -d numbers           ];then $(MAKE) -C numbers           $(x); fi
	if [ -d coding            ];then $(MAKE) -C coding            $(x); fi
	if [ -d coding/zlib       ];then $(MAKE) -C coding/zlib       $(x); fi
	if [ -d coding/img        ];then $(MAKE) -C coding/img        $(x); fi
	if [ -d graph/graph_types ];then $(MAKE) -C graph/graph_types $(x); fi
	if [ -d graph/graph_alg   ];then $(MAKE) -C graph/graph_alg   $(x); fi
	if [ -d graph/graph_draw  ];then $(MAKE) -C graph/graph_draw  $(x); fi
	if [ -d graph/graph_iso   ];then $(MAKE) -C graph/graph_iso   $(x); fi
	if [ -d geo/plane         ];then $(MAKE) -C geo/plane         $(x); fi
	if [ -d geo/plane_alg     ];then $(MAKE) -C geo/plane_alg     $(x); fi
	if [ -d geo/d3_geo        ];then $(MAKE) -C geo/d3_geo        $(x); fi
	if [ -d graphics/window   ];then $(MAKE) -C graphics/window   $(x); fi
	if [ -d graphics/graphwin ];then $(MAKE) -C graphics/graphwin $(x); fi
	if [ -d graphics/anim     ];then $(MAKE) -C graphics/anim     $(x); fi
	if [ -d C:/         ];then \
	if [ -d graphics/mswin ]; then $(MAKE) -C graphics/mswin $(x);fi;else \
	if [ -d graphics/x11   ]; then $(MAKE) -C graphics/x11 $(x); fi; fi 
	if [ -d graphics/xx    ]; then $(MAKE) -C graphics/xx clean; fi 
	if [ -d graphics/geowin ];then $(MAKE) -C graphics/geowin  $(x); fi


