#!/bin/bash

# Add new languages here, space separated and using the ID for `crowdin pull`
LANGUAGES="cs da de es-ES fr hu it ja ko lt nl no pl pt-PT pt-BR ro ru ry tr uk zh-CN zh-TW"

ARG=''
for LANGUAGE in $LANGUAGES; do
	ARG+="-l $LANGUAGE "
done
crowdin pull $ARG
