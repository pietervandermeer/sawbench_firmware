all: test_env

clean:
	rm test_env

test_env: test_env.cpp SimpleEnvelope/SimpleEnvelope.cpp SimpleEnvelope/SimpleEnvelope.h
	g++ test_env.cpp -DUNITTEST SimpleEnvelope/SimpleEnvelope.cpp -o test_env -ISimpleEnvelope/
