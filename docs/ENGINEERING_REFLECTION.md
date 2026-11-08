# NurseryLedger — Engineering Reflection

## From a Childhood Memory to a Software Project

When I received this assignment, I didn't immediately start coding. I spent a considerable amount of time brainstorming because I wanted to build something that felt meaningful rather than simply choosing an idea I already knew how to implement.

My first thought was a volunteer management application, inspired by my experience organizing developer community events. But I eventually found myself thinking about something completely different: my family's nursery.

My grandfather helped finance the business, and my mother worked incredibly hard to overcome its financial difficulties and keep it running. It supported our family for a period before eventually closing in 2020.

I remember how stressful managing its finances could be. On one occasion, some bills went missing, and my family had to cover the difference out of pocket until the issue was resolved.

That memory became the motivation behind NurseryLedger.

I wanted to build something that could have made those administrative responsibilities easier, while giving myself the opportunity to approach a real problem through software engineering.

## Defining the Scope

My original vision was much larger than a fee-tracking application. I imagined a complete nursery management system covering employee salaries, inventory, operational expenses, and financial reporting.

However, I realized that attempting everything at once would make it difficult to properly define and verify the business rules.

I decided to focus Version 1 on one specific area: **the money a nursery is owed and the money it actually receives.**

The application supports registration, monthly fee obligations, payments, outstanding balances, credit allocation, refunds, and transaction history.

One decision I particularly valued was separating the billing period from the actual payment receipt date. A November fee might be paid in December, and preserving both facts is important for understanding the nursery's finances.

I also simplified enrollment dates to the 1st and 15th of each month. This allowed me to implement a consistent billing policy rather than making assumptions about more complicated enrollment scenarios.

The larger vision remains in the roadmap, while the first release concentrates on defined and testable financial behavior.

## Architecture and Technical Decisions

I developed NurseryLedger in C++17 for Linux, using CMake for the build and C++ tests.

I generally lean toward Python, so working extensively with C++ was an interesting opportunity to strengthen my understanding of its structure and design.

The architecture evolved during development. I separated the application into a CLI for user interaction, a Ledger responsible for enforcing financial rules, and a Storage component for persistence.

I used structures for straightforward data models and classes for components responsible for operations and business constraints. This was also an opportunity to explore a design approach I hadn't used extensively before.

For monetary values, I chose 64-bit integer cents rather than floating-point numbers to avoid rounding problems in ordinary financial calculations.

I maintained a custom Date implementation in C++17. With hindsight, I would spend more time evaluating the available standard-library alternatives before making that decision.

Persistence was more challenging than I initially expected. CSV seemed straightforward, but reliably saving and restoring financial records introduced several problems.

I eventually implemented versioned CSV snapshots, allowing a new dataset to be written separately before the committed snapshot pointer is updated.

## Testing, Debugging, and Reliability

Testing was probably my favorite part of the project.

After spending so much time implementing the application in C++, returning to Python felt refreshing. I particularly enjoyed thinking about unusual situations and trying to find ways the program could behave incorrectly.

I built six C++ unit-test suites around the main business rules and seventeen Python component-testing scenarios that exercise the application from the outside.

These tests cover duplicate billing, partial payments, overpayments, refunds, invalid input, persistence across restarts, and simulated storage failures.

One memorable issue occurred during the snapshot implementation. The application successfully saved a €200 payment against a €150 bill, but after restarting, the available €50 credit appeared to disappear.

With debugging assistance, I examined the saved records and discovered that the payment still existed in the snapshot. The problem was that some loading paths were pointing to the wrong directory.

Correcting those paths and verifying that the payment and credit survived the restart reinforced something important: **writing data successfully does not guarantee that an application can reliably restore it.**

I used AI assistance during parts of development for brainstorming, understanding C++ concepts, discussing architecture, and debugging implementation errors. I also learned that suggestions need to be understood and verified rather than accepted simply because they appear correct.

## Production Readiness and Scalability

Before this project, much of my development experience came from university assignments. I hadn't fully appreciated the difference between building something that works and building something people can depend on.

For example, NurseryLedger currently saves changes during normal exit. If the application terminates unexpectedly, recent transactions may be lost.

That is a much more serious issue when the data represents actual financial records.

I also started wondering what would happen if the project grew from a small nursery into a school with thousands of students.

I haven't benchmarked the current implementation at that scale, so I cannot claim it would perform reliably. Larger transaction histories, repeated snapshot writes, concurrent access, and data recovery would all need further investigation.

For a larger deployment, I would evaluate transactional database storage, stronger recovery guarantees, transaction identifiers, access controls, and performance testing.

The application achieves its defined functional scope, but there are still important operational safeguards required before unrestricted real-world deployment.

## Looking Back

If I started NurseryLedger again, I would think about persistence and failure scenarios earlier, and spend more time comparing technical alternatives before committing to an implementation.

What I'm proudest of isn't simply that the application works. It's that I took an idea connected to my own experience, explored it beyond my usual comfort zone, and gradually started asking more difficult questions about its design.

The project exposed me to a side of software engineering I hadn't experienced as deeply before: defining business rules, protecting financial data, testing failure scenarios, understanding trade-offs, and recognizing the limits of an implementation.

**I started by asking myself what I could build. By the end, I was asking whether people could actually trust what I had built.**

For me, that change in perspective is the most valuable result of NurseryLedger.
