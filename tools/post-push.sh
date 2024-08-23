#!/bin/sh

# This is a script simply triggers the 'trigger_from_cli' event
# on github.com/csprite/builds repo, and can be triggered after
# a `git xpush`
# Just ensure, xpush is defined in your repo's `.git/config` file:
# [alias]
#     xpush = "!if git merge-base --is-ancestor HEAD @{u}; then echo 'Nothing to push'; else git push $1 $2 && ./tools/post-push.sh; fi"

set -e

API_TOKEN=$(pass show github/csprite-dispatch-build)
COMMIT_BRANCH=$(git rev-parse --verify --abbrev-ref HEAD)
COMMIT_HASH=$(git rev-parse --verify HEAD)
COMMIT_HASH_SHORT=$(git rev-parse --verify --short HEAD)
COMMIT_MESSAGE=$(git log --format=%B -n 1 $COMMIT_HASH | xargs -0)

if [ -z "$API_TOKEN" ]; then
	echo "GitHub API token is empty, couldn't dispatch workflow event!"
	exit 1
elif [ -z "$COMMIT_HASH" ]; then
	echo "Failed to retrieve latest commit hash!"
elif [ -z "$COMMIT_MESSAGE" ]; then
	echo "Failed to retrieve latest commit message!"
elif [ -z "$COMMIT_BRANCH" ]; then
	echo "Failed to retrieve latest commit branch!"
fi

curl \
	-X POST \
	-H "Accept: application/vnd.github+json" \
	-H "Authorization: Bearer $API_TOKEN" \
	-H "X-GitHub-Api-Version: 2022-11-28" \
	https://api.github.com/repos/csprite/builds/dispatches \
	-d "{ \"event_type\": \"trigger_from_cli\", \"client_payload\": { \"commit_branch\": \"$COMMIT_BRANCH\", \"commit_message\": \"$COMMIT_MESSAGE\", \"commit_hash\": \"$COMMIT_HASH\", \"commit_hash_short\": \"$COMMIT_HASH_SHORT\" } }"

exit 0

