# minitask 8
import asyncio
import time

async def perform_task(duration, task_name):
    print(task_name, 'starting')
    await asyncio.sleep(duration)
    print(task_name, 'is done')

async def main():
    await asyncio.gather(
        perform_task(3, 'boiling kettle'),
        perform_task(2, 'cleaning cups')
    )
    
s = time.perf_counter()
asyncio.run(main())
elapsed = time.perf_counter() - s
print(f"Executed in {elapsed:0.2f} seconds.")
