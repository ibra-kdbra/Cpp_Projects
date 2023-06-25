
all: docker build test

docker: ## Get docker image
	docker build -t projects/cordic:sim -f Dockerfile .

test-docker: ## Verify docker image is working
	docker run -t projects/cordic:sim --version

build: ## Build simulation
	docker run -ti -v ${PWD}:/work --user mal projects/cordic:sim -Wall --cc cordic.v --exe sim.cpp
	docker run -ti --entrypoint /bin/make -v ${PWD}:/work --user mal projects/cordic:sim -j -C obj_dir/ -f Vcordic.mk Vcordic

.PHONY: tags
size:
	docker images --format "table {{.Repository}}\t{{.Tag}}\t{{.Size}}" projects/cordic

test: build ## Test docker image
	@./obj_dir/Vcordic 330

.PHONY: ssh
ssh: ## SSH into docker image
	@docker run --init -it --rm --entrypoint=bash -e DISPLAY=$(ipconfig getifaddr en0):0 projects/cordic:sim

.PHONY: stop-docker
stop-docker: ## Kill running verilator container
	@docker rm -f cordic || true

.PHONY: stop-all
stop-all: ## Kill ALL running docker containers
	@docker-clean stop

clean-docker: stop-all ## Clean docker image and stop all running containers
	docker rmi projects/cordic:sim || true

clean:
	rm -rf obj_dir

# Absolutely awesome: http://marmelab.com/blog/2016/02/29/auto-documented-makefile.html
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

.DEFAULT_GOAL := help