import {spawn} from 'node:child_process';
import {createRequire} from 'node:module';
const require=createRequire(import.meta.url);
const args=process.argv.slice(2);const nextArgs=['dev','--webpack'];let host='127.0.0.1';
for(let i=0;i<args.length;i++){if(args[i]==='--host'||args[i]==='--hostname')host=args[++i];else if(args[i]==='--port')nextArgs.push('--port',args[++i]);else if(args[i]!=='--strictPort')throw new Error('Unsupported dev argument');}
const child=spawn(process.execPath,[require.resolve('next/dist/bin/next'),...nextArgs,'--hostname',host],{stdio:'inherit'});
for(const signal of ['SIGINT','SIGTERM'])process.on(signal,()=>child.kill(signal));
child.on('exit',code=>process.exit(code??1));
