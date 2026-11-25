# Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
# SPDX-License-Identifier: GPL-2.0-only
#
# Q-Swarm: the org's standard targets (swarm-routing/.github#11). Every org
# repo has these same target names, with this repo's own commands behind them;
# `org-sync check` verifies the names are present, never the recipes. Add
# repo-only targets above the org-tools block at the end: that block is the
# org's, refreshed by org-bot (swarm-routing/.github#615). `make help` lists
# everything with a `## ` note.

NS3_IMAGE ?= ghcr.io/swarm-routing/ns3-dev:latest-debug
PLATFORM  ?=
# container kind: the image's Dockerfile and its build arguments
DOCKERFILE ?= Dockerfile
BUILD_ARGS ?=
# swarm-core's core/ at the release swarm-core.lock names (its tag): the include
# check's dependency, and core-test's swarm-core when neither VCPKG_ROOT nor
# SWARM_CORE_DIR is given. Bumped with the lock by org-bot, never by hand.
SWARM_CORE_SRC ?= $(HOME)/.cache/swarm-routing/swarm-core
SWARM_CORE_REF ?= v2.1.0

.DEFAULT_GOAL := help
.PHONY: help build test lint format check clean shell ci headers swarm-core core-test dist

help: ## list the targets
	@awk 'BEGIN{FS=":.*## "} /^[a-zA-Z0-9_-]+:.*## /{printf "  %-16s %s\n", $$1, $$2}' $(MAKEFILE_LIST)

build: ## build what this repo ships
	if [ -d ns3 ]; then docker run --rm -v "$(CURDIR):/src:ro" $(NS3_IMAGE) ns3-build-contrib q-swarm=/src/ns3; else echo "make build: no ns3/ module yet"; fi

test: core-test ## the fast tests CI runs on every PR
	if [ -d ns3 ]; then docker run --rm -v "$(CURDIR):/src:ro" $(NS3_IMAGE) ns3-build-contrib --test q-swarm=/src/ns3; else echo "make test: no ns3/ module yet"; fi

# the org's .clang-format, as core.yml checks it (a repository .clang-format wins)
CLANG_STYLE = $(if $(wildcard .clang-format),file,file:$(ORG_DIR)/.clang-format)

lint: org-tools ## static checks, no changes
	git ls-files "*.h" "*.cc" "*.cpp" "*.hpp" | xargs -r clang-format-18 --style="$(CLANG_STYLE)" --dry-run --Werror

format: org-tools ## apply the formatters
	git ls-files "*.h" "*.cc" "*.cpp" "*.hpp" | xargs -r clang-format-18 --style="$(CLANG_STYLE)" -i

clean: ## remove build outputs
	rm -rf build dist

shell: ## an interactive shell in the dev image with this repo at /src
	@command -v docker >/dev/null || { echo "make shell: docker not found (Docker Desktop, Colima, or docker.io)" >&2; exit 127; }
	docker run --rm -it -v "$(CURDIR):/src" -w /src $(if $(PLATFORM),--platform $(PLATFORM)) $(NS3_IMAGE) bash

swarm-core:  # swarm-core at SWARM_CORE_REF (swarm-core.lock's tag)
	@[ -d "$(SWARM_CORE_SRC)/.git" ] || git init -q "$(SWARM_CORE_SRC)"
	@git -C "$(SWARM_CORE_SRC)" fetch -q --depth 1 https://github.com/swarm-routing/swarm-core $(SWARM_CORE_REF) && git -C "$(SWARM_CORE_SRC)" checkout -q --detach FETCH_HEAD

headers: org-tools swarm-core ## licence headers (org rule 1): the Go gate CI runs, in docker or built with go
	"$(ORG_DIR)/tools/checks/check-headers.sh" --dual core/ --core core/ --core-dep "$(SWARM_CORE_SRC)/core" .

check: lint test headers org-sync-check ## everything CI gates on

ci: check ## exactly what CI runs

# core/ takes swarm-core from its vcpkg port (vcpkg.json, vcpkg-configuration.json)
# when VCPKG_ROOT (a vcpkg checkout) is given, as core.yml; SWARM_CORE_DIR=<a
# swarm-core checkout>/core builds on it instead; with neither, on swarm-core's
# core/ at SWARM_CORE_REF (the lock's tag), so `make check` needs no vcpkg.
VCPKG_ROOT ?=
SWARM_CORE_DIR ?=
# one build directory per source: CMake reads the vcpkg toolchain only on a
# directory's first configure, so the two never share one
CORE_BUILD = build/core$(if $(VCPKG_ROOT),,-local)
CORE_SWARM = $(or $(SWARM_CORE_DIR),$(SWARM_CORE_SRC)/core)
core-test: $(if $(VCPKG_ROOT)$(SWARM_CORE_DIR),,swarm-core) ## core/ alone, no simulator: CMake + ctest with CI's strict warnings
	cmake -S core -B $(CORE_BUILD) $(if $(VCPKG_ROOT),-DSWARM_CORE_DIR= -DCMAKE_TOOLCHAIN_FILE=$(VCPKG_ROOT)/scripts/buildsystems/vcpkg.cmake,-DSWARM_CORE_DIR=$(CORE_SWARM)) \
		&& cmake --build $(CORE_BUILD) && ctest --test-dir $(CORE_BUILD) --output-on-failure

# The release asset for swarm-routing/.github's reusable-release.yml, which sets
# RELEASE_VERSION (locally: .cz.toml's version). A git archive of the commit, so
# only tracked files, deterministically: q-swarm-<ver>/ with core/ and its
# LICENSE, swarm-core.lock, docs/, the licence files, README.md, AGENTS.md, the
# Makefile and CHANGELOG.md (once the first bump writes it). The vcpkg port
# q-swarm-core builds core/ from it.
RELEASE_VERSION ?= $(shell sed -n 's/^version = "\(.*\)"$$/\1/p' .cz.toml)
dist: ## the release asset in dist/: q-swarm-<ver>.tar.gz
	rm -rf dist && mkdir -p dist
	git archive --format=tar.gz --prefix=q-swarm-$(RELEASE_VERSION)/ -o dist/q-swarm-$(RELEASE_VERSION).tar.gz HEAD \
		$$(git ls-tree --name-only HEAD core swarm-core.lock vcpkg.json vcpkg-configuration.json docs LICENSE LICENSE-MIT LICENSING.md README.md AGENTS.md Makefile CHANGELOG.md)
	tar -tzf dist/q-swarm-$(RELEASE_VERSION).tar.gz | grep -qx 'q-swarm-$(RELEASE_VERSION)/core/LICENSE'
	tar -tzf dist/q-swarm-$(RELEASE_VERSION).tar.gz | grep -qx 'q-swarm-$(RELEASE_VERSION)/swarm-core.lock'

# org-sync:begin org-tools
# The org tooling (check-headers, org-sync): a shallow clone, refreshed to ORG_REF,
# by default the template commit .github/org-sync.lock pins, as CI checks (#586).
ORG_DIR   ?= $(HOME)/.cache/swarm-routing/dotgithub
ORG_REF   ?= $(or $(shell sed -nE 's/^commit = "([0-9a-f]{40})"[[:space:]]*$$/\1/p' .github/org-sync.lock 2>/dev/null),main)
# make pre-push TITLE='type(scope): subject': the PR title rule of reusable-pr-title.yml
export TITLE
TITLE_RE  := ^(feat|fix|docs|style|refactor|perf|test|build|ci|chore|revert)(\([a-z0-9._/-]+\))?!?: [^A-Z ](.*[^ ])?$$

.PHONY: org-tools org-sync-check pre-push

org-tools:  # the same templates CI checks against: refreshed to ORG_REF every time
	@[ -d "$(ORG_DIR)/.git" ] || { git init -q "$(ORG_DIR)" && git -C "$(ORG_DIR)" remote add origin https://github.com/swarm-routing/.github; }
	@git -C "$(ORG_DIR)" fetch -q --depth 1 origin $(ORG_REF) && git -C "$(ORG_DIR)" checkout -q --detach FETCH_HEAD

org-sync-check: org-tools ## org-managed files match the org templates
	python3 -I "$(ORG_DIR)/tools/org-sync/org_sync.py" check .

pre-push: check ## before every push: make check, then TITLE='type(scope): subject' (the PR title)
	@[ -n "$$TITLE" ] || { echo "make pre-push: pass TITLE='<the PR title>'" >&2; exit 2; }
	@printf '%s\n' "$$TITLE" | grep -Eq '$(TITLE_RE)' || { echo "PR title is not a lowercase Conventional Commit: $$TITLE" >&2; exit 1; }
	@echo "pre-push: ok ($$TITLE)"

.PHONY: doctor
doctor: ## this machine's tools against the versions CI runs (the org's tools/versions.env, go.mod), and the hosts the network steps need
	@$(MAKE) -s org-tools || echo "doctor: could not refresh $(ORG_DIR) (offline?): using it as it is" >&2
	@"$(ORG_DIR)/tools/doctor/doctor.sh" .
# org-sync:end org-tools
