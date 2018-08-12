# lazyEvaluation
Implementation of the Lazy Evalutation pattern for some Computations of an Element.
We're using a Manager that will get the computation tasks and store their result until it is requested.
It is possible to cancel tasks. (it will only work if the tasks aren't started, otherwise we let the dedicated Thread finish)
An Element will release its Tasks from the Manager in its destructor if he has schedule any.
