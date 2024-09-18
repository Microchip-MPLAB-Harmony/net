import { useCallback, useRef } from "react";
const max_render = 10;
export const useRenderCounter = () => {
  const counterRef = useRef(max_render);
  const callBackRef = useRef<VoidFunction>();

  const executeCallback = useCallback(async (cb: VoidFunction) => {
    callBackRef.current = cb;
    if (counterRef.current < 0) {
      counterRef.current = max_render;
      window.scrollTo(0, 100);
      return;
    }
    cb();
    await new Promise((resolve) => setTimeout(resolve, 500));
    counterRef.current -= 1;
    executeCallback(cb);
  }, []);

  //     useEffect(() => {
  //       window.addEventListener('resize', () => {
  //         console.log('===> sss')
  //         executeCallback(callBackRef.current!)
  //       });

  //          return () => {
  // window.removeEventListener('resize', () => {})
  //          }
  //     },[executeCallback])

  return { executeCallback };
};
